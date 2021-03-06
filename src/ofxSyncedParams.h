//
//  ofxSyncedParams
//  based on datGuiController originally by ofZach

#pragma once

#include "ofMain.h"
#include "ofxJSON.h"
#include "ofxGui.h"
#include  "json.h"

//TODO save settings via remote, 0.8.4 version possible?, two classes remote vs. client
//TODO p5 js example, combine with ofParam example, simplify datGui parser
//TODO memory leak? params are created by ofxSyncedParams if setupFromJson is used

class ofxSyncedParams {
public:
	ofxSyncedParams();

	//to setup your client (from a ofParamterGroup or a ofxGui)
	void setupFromGui(ofxPanel & gui);
	void setupFromParamGroup(ofParameterGroup & group);

	//or to use ofxSyncedParams for a remote GUI (and create an parameter group  from Json)
	ofxGuiGroup * setupFromJson(Json::Value & jsonInit);

	//parse the full parameter set to JSON (to initialize your remote GUI)
	string parseParamsToJson ( );

	//parse incoming updates from the remote GUI and update your parameters
	void updateParamFromJson( ofxJSONElement json );

	ofEvent<std::string> paramChangedE;
protected:
    bool bSetup;
    ofParameterGroup * rootGroup;

    //helper method for parseParamsToJson()
    Json::Value parseParamGroup( ofParameterGroup & subGroup,  bool bInnerGroup); //TODO use pointer

    //event for changed parameters
    std::string changedParamInJson;
    void parameterChanged( ofAbstractParameter & parameter);

    //helper methods for setupFromJson()
    void addToGroup(string & name, Json::Value & obj, ofParameterGroup * group);
	void unfurl(Json::Value & obj, ofParameterGroup * group);
};
