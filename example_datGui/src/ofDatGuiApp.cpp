#include "ofDatGuiApp.h"

string jsonString = "";

bool onUpdate = false;
bool eInitRequest = false;

ofxJSONElement paramUpdate;

//--------------------------------------------------------------
void ofDatGuiApp::setup() {
	ofSetLogLevel(OF_LOG_VERBOSE);
	ofSetBackgroundColor(0);

	//create GUI group
	gui.setup("gui","settings.xml",50,50);
	group1.setName("group1");
	group1.add(intParam1.set("myInt",100,0,100));
	group1.add(floatParam1.set("myFloat",0.5,0,1));
	group1.add(toggle1.set("myToggle",false));
	gui.add(group1);
	group2.setName("group2");
	group2.add(intParam2.set("myInt",0,0,100));
	group2.add(floatParam2.set("myFloat",0,0,10));
	group2.add(toggle2.set("myToggle",true));
	group21.setName("group2a");
	group21.add(color1.set("myColor",ofColor::oliveDrab,ofColor(0,0),ofColor(255,255)));
	group2.add(group21);
	gui.add(group2);
	gui.loadFromFile("settings.xml");

	//setup sync for GUI
	paramSync.setupFromGui(gui);

	//listen to parameter changes from ofxSynchedParams
	ofAddListener(paramSync.paramChangedE,this,&ofDatGuiApp::parameterChanged);

	//setup web socket server
	ofxLibwebsockets::ServerOptions options = ofxLibwebsockets::defaultServerOptions();
	options.port = 9092;
	options.bUseSSL = false; // you'll have to manually accept this self-signed cert if 'true'!
	options.documentRoot = ofToDataPath("web"); // that is the default anyway but so you know how to change it :)
	server.setup( options );
	server.addListener(this);
}

//--------------------------------------------------------------
void ofDatGuiApp::parameterChanged( std::string & paramAsJsonString ){
//	ofLogVerbose("ofDatGuiApp::parameterChanged");
	if(!onUpdate)
		server.send( paramAsJsonString );
}

//--------------------------------------------------------------
void ofDatGuiApp::update() {
	//webUi
	if(eInitRequest){
		eInitRequest = false;
		jsonString = paramSync.parseParamsToJson();
		ofLogVerbose("ofDatGuiApp::update") << "parsed json string:" << jsonString;
		server.send(jsonString);
	}

	if(onUpdate){
		paramSync.updateParamFromJson(paramUpdate);
		onUpdate = false;
	}
}

//--------------------------------------------------------------
void ofDatGuiApp::draw() {
	ofSetColor(255);
    ofDrawBitmapString("Press Return to start the web UI.", 10,20);
	gui.draw();
}

//--------------------------------------------------------------
void ofDatGuiApp::keyPressed(int key){
	if(key == OF_KEY_RETURN){
		string url = "http";
		if ( server.usingSSL() ){
			url += "s";
		}
		url += "://localhost:" + ofToString( server.getPort() );
		ofLaunchBrowser(url);
	}
}

//--------------------------------------------------------------
void ofDatGuiApp::onMessage( ofxLibwebsockets::Event& args ){
    // trace out string messages or JSON messages!
    if ( !args.json.isNull() ){
    	ofLogVerbose("ofDatGuiApp::onMessage") << "json message: " << args.json.toStyledString() << " from " << args.conn.getClientName();

        if(args.json["type"]=="initRequest"){
        	eInitRequest = true;
        }else if(!onUpdate){
			paramUpdate = args.json;
			onUpdate = true;
        }
    }
}
