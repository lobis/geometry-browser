/*
    Author: Luis Antonio Obis Aparicio (luis.antonio.obis@gmail.com)
    GitHub: https://github.com/lobis/geometry-browser
*/

#include "THttpServer.h"
#include "TGeoManager.h"
#include "TTimer.h"
#include "TNamed.h"
#include "TUUID.h"
#include "TTree.h"

const char *PORT_ENV_VAR = "GEOMETRYBROWSER_PORT"; // if this environment variable is specified, the port will be set to that (make sure its a valid port)
const char *PORT_DEFAULT = "8090";                 // this is the default port
const char *PORT = (!getenv(PORT_ENV_VAR)) ? PORT_DEFAULT : getenv(PORT_ENV_VAR);

THttpServer *serv;

string gGeometryFilename = "test/geometry/BabyIAXO.root"; // default value
int interval = 500;                                       // interval in ms to do automatic updates
auto emptyObject = new TNamed("Undefined", "Undefined");
TEveGeoShapeExtract *gGSE;
TFile *file;
auto previousUUID = TUUID();
regex extensionRe("(.*)\\.[^.]+$");
regex pathRe("^[^/]+/");

string getExtension()
{
    // extension should only be ".root" or ".gdml"
    if (gGeometryFilename.length() <= 5)
    {
        return "";
    }
    return gGeometryFilename.substr(gGeometryFilename.length() - 5);
}

string cleanFilename(const string &filename = gGeometryFilename)
{
    string result, replace;
    // remove all extensions at the end of the file and all the leading directories ((/?)this/is/a/test.ext1.ext2 -> test)
    replace = filename;
    while (kTRUE)
    {
        result = regex_replace(replace, extensionRe, "$1");
        result = regex_replace(result, pathRe, "$1");
        if (replace == result)
        {
            // once the replacement does nothing, we return
            break;
        }
        replace = result;
    }

    return result;
}

Bool_t useSingleGeometryName = kTRUE; // if this is set to true, all geometry objects will be named "Geometry"
void ToggleSingleGeometryName(Bool_t value)
{
    useSingleGeometryName = value;
    const char *objectName = (useSingleGeometryName) ? "Geometry" : cleanFilename().c_str();
    if (gGSE)
    {
        gGSE->SetName(objectName);
    }
}

void LoadGeometry()
{
    auto onFail = []() {
        if (gGSE)
        {
            serv->Register("", emptyObject);
            serv->Unregister(gGSE);
            gGSE = nullptr;
        }
    };

    cout << "DEBUG: Attempting to load file '" << gGeometryFilename << "'" << endl;

    Bool_t loadedTree = kFALSE;

    if (file)
    {
        delete file;
    }
    // read "gGeometryFilename", it can be either a .gdml file or a .root file containing (atleast) a TGeoManager element
    const string extension = getExtension();
    if (extension == ".root")
    {
        file = TFile::Open(gGeometryFilename.c_str());
        if (!file)
        {
            cout << "ERROR: no file named: '" << gGeometryFilename << "'" << endl;
            onFail();
            return;
        }
        // need to extract the TGeoManager object, we iterate over all keys and get the first one it matches
        TIter next(file->GetListOfKeys());
        TKey *key;
        TTree *T;
        Bool_t foundGeometry = kFALSE;
        while ((key = (TKey *)next()))
        {
            if (!strcmp(key->GetClassName(), "TGeoManager") && !foundGeometry)
            {
                const auto keyName = key->GetName();
                cout << "INFO: Reading key '" << keyName << "' (" << key->GetClassName() << ") from file '" << gGeometryFilename << "'" << endl;
                file->GetObject(keyName, gGeoManager);
                foundGeometry = kTRUE;
            }
            if (!strcmp(key->GetClassName(), "TTree"))
            {
                const auto keyName = key->GetName();
                cout << "INFO: Reading key '" << keyName << "' (" << key->GetClassName() << ") from file '" << gGeometryFilename << "'" << endl;
                TTree *tree;
                file->GetObject(keyName, tree);
                tree->SetName(keyName);
                serv->Register("", tree);
                loadedTree = kTRUE;
            }
        }
        previousUUID = file->GetUUID();
        if (!foundGeometry)
        {
            cout << "ERROR: Did not find a geometry (TGeoManager) in file '" << gGeometryFilename << "'" << endl;
            onFail();
        }
    }
    else if (extension == ".gdml")
    {
        TGeoManager::Import(gGeometryFilename.c_str());
    }
    else
    {
        cout << "file: '" << gGeometryFilename << "' has no valid extension (.gdml or .root)" << endl;
        onFail();
        return;
    }

    if (!gGeoManager)
    {
        cout << "ERROR: no TGeoManager found in file: '" << gGeometryFilename << "'" << endl;
        onFail();
        return;
    }
    gGSE = (TEveGeoShapeExtract *)gGeoManager;

    // remove extension and leading directories. This is requiered to use the API (or I didn't find a workaround)
    ToggleSingleGeometryName(useSingleGeometryName);
    serv->Register("", gGSE);
    serv->Unregister(emptyObject);

    if (!loadedTree && file)
    {
        delete file;
    }
}

void Update(string filename = gGeometryFilename)
{
    cout << "INFO: Updating geometry manually (filename = '" << filename << "')" << endl;
    // updating global filename
    gGeometryFilename = filename;

    LoadGeometry();
}

void AutoUpdate()
{
    // This is run continously in the background (via TTimer), if the UUID of the file changes, we read it again. (
    // TODO: the contents of a file could change and the UUID may remain the same, find a better way to look for changes
    if (getExtension() != ".root")
    {
        return;
    }
    TFile *file = TFile::Open(gGeometryFilename.c_str());
    if (!file)
    {
        return;
    }
    if (!file->GetUUID().Compare(previousUUID))
    {
        //cout << "UUIDs are equal" << endl;
        delete file;
        return;
    }

    cout << "INFO: Automatically updating geometry since file probably changed" << endl;
    delete file;
    LoadGeometry();
}

void GeometryBrowser()
{
    serv = new THttpServer(Form("http:%s?top=%s", PORT, "Geometry Viewer"));

    serv->RegisterCommand("/UpdateGeometryFile", "Update(\"%arg1%\")", "button;rootsys/icons/ed_open.png");
    serv->RegisterCommand("/Refresh", "LoadGeometry()", "button;rootsys/icons/ed_open.png");
    serv->RegisterCommand("/EnableSingleGeometryName", "ToggleSingleGeometryName(kTRUE)", "rootsys/icons/ed_execute.png");
    serv->RegisterCommand("/DisableSingleGeometryName", "ToggleSingleGeometryName(kFALSE)", "rootsys/icons/ed_delete.png");
    //serv->Hide("/EnableSingleGeometryName");
    //serv->Hide("/DisableSingleGeometryName");

    TTimer *timer = new TTimer("AutoUpdate()", interval);
    timer->TurnOn();
}
