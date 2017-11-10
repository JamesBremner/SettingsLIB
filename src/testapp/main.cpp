#include <Windows.h>
#include <iostream>
#include "isettingsservice.h"
#include "../settingslib/settingsservice.h"

using namespace std;

static const char* APP_GROUP = "app_group";

void boolTest( ISettingsService* settings ) {
    const char* FIRSTRUN_SETTING = "first_run";
    cout << "Bool Test: ";
    bool appFirstRun = settings->getBool( APP_GROUP, FIRSTRUN_SETTING, true );
    if (appFirstRun) {
        cout << "Running first time init code" << endl;
        settings->setBool( APP_GROUP, FIRSTRUN_SETTING, false );
    }
    else
        cout << "App allready inited" << endl;
}

void intTest( ISettingsService* settings ) {
    const char* RUN_COUNT = "run_count";
    int runCount = settings->getInt( APP_GROUP, RUN_COUNT, 1 );
    std::string runCountStr = settings->getString( APP_GROUP, RUN_COUNT, "1" );

    cout << "Int Test: This is " << runCount << " [as_str: '" << runCountStr << "'] run of this app" << endl;
    ++runCount;
    settings->setInt( APP_GROUP, RUN_COUNT, runCount );
}

void floatTest( ISettingsService* settings ) {
    const char* FLOATVAL = "speed";
    const char* RUN_COUNT = "run_count";

    float   f = settings->getFloat( APP_GROUP, FLOATVAL, 0.5f );
    std::string fstr = settings->getString( APP_GROUP, FLOATVAL, "0" );
    cout << "CurrentFloat: " << f << "[ as string: '" << fstr << "']" << endl;

    int runCount = settings->getInt( APP_GROUP, RUN_COUNT, 1 );
    if (runCount & 1)
        settings->setFloat( APP_GROUP, FLOATVAL, 1.f );
    else
        settings->setFloat( APP_GROUP, FLOATVAL, 0.f );
}

void strTest( ISettingsService* settings ) {
    const char* STRVAL = "strval";
    const char* RUN_COUNT = "run_count";
    int runCount = settings->getInt( APP_GROUP, RUN_COUNT, 1 );

    std::string str = settings->getString( APP_GROUP, STRVAL, "empty" );
    cout << "Sample str: " << str << endl;

    if (runCount & 1)
        settings->setString( APP_GROUP, STRVAL, "sample_str1" );
    else
        settings->setString( APP_GROUP, STRVAL, "sample_str2" );
}

int main() {
    ISettingsService::Ref   settings = SettingsService::createInstance();
    settings->onInit( /* Pass here data Storage */);

    boolTest( settings.get() );
    intTest( settings.get() );
    floatTest( settings.get() );
    strTest( settings.get() );

    settings->onShutdown(); // Last call to save settings
}

/* EOF */