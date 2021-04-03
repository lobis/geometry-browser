/*
    Author: Luis Antonio Obis Aparicio (luis.antonio.obis@gmail.com)
    GitHub: https://github.com/lobis/geometry-browser
*/

#include "THttpServer.h"
#include "TGeoManager.h"
#include "TTimer.h"
#include "TNamed.h"
#include "TUUID.h"

const int PORT = 8090; // the application will be on this port
THttpServer *serv;

string gGeometryFilename = "test/BabyIAXO.root"; // default value
int interval = 2000;                             // interval in ms to do automatic updates
auto emptyObject = new TNamed("Undefined", "Undefined");
TEveGeoShapeExtract *gGSE = nullptr;
auto previousUUID = TUUID();

string getExtension()
{
    // extension should only be ".root" or ".gdml"
    if (gGeometryFilename.length() <= 5)
    {
        return "";
    }
    return gGeometryFilename.substr(gGeometryFilename.length() - 5);
}
void loadGeometry()
{
    auto onFail = []() {
        if (gGSE)
        {
            serv->Register("", emptyObject);
            serv->Unregister(gGSE);
            gGSE = nullptr;
        }
    };

    // read "gGeometryFilename", it can be either a .gdml file or a .root file containing (atleast) a TGeoManager element
    string extension = getExtension();
    if (extension == ".root")
    {
        TFile *file = TFile::Open(gGeometryFilename.c_str());
        if (!file)
        {
            cout << "ERROR: no file named: '" << gGeometryFilename << "'" << endl;
            onFail();
            return;
        }
        file->GetObject("geo", gGeoManager);
        previousUUID = file->GetUUID();
        delete file;
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

    gGSE->SetName(gGeometryFilename.c_str());
    serv->Register("", gGSE);
    serv->Unregister(emptyObject);
}

void Update(string filename = gGeometryFilename)
{
    cout << "INFO: Updating geometry manually (filename = '" << filename << "')" << endl;
    // updating global filename
    gGeometryFilename = filename;

    loadGeometry();
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
    loadGeometry();
}

void GeometryBrowser()
{
    serv = new THttpServer(Form("http:%d?top=%s", PORT, "Geometry Viewer"));

    serv->RegisterCommand("/UpdateGeometryFile", "Update(\"%arg1%\")", "button;rootsys/icons/ed_open.png");

    TTimer *timer = new TTimer("AutoUpdate()", interval);
    timer->TurnOn();
}
