#include "tableInterface.h"

//--------------------- instruction box -----------

void instructionBox::setup()
{
	rodInst.loadImage("images/instructions/rods.jpg");
	buildInst.loadImage("images/instructions/build.jpg");
	rodSel.setup(300,300,"images/instructions/rodBut.jpg");
	buildSel.setup(300,300,"images/instructions/buildBut.jpg");
	resetBut.setup("Return to experiment selection","fonts/Arial.ttf",15);
	resetBut.arial.setMode(OF_FONT_TOP);
}
	
void instructionBox::draw(int x, int y, int w, int h)
{
	if(!rodSel.pressed()&&!buildSel.pressed()){
		rodSel.draw(x,y+(h-rodSel.h)/2);
		buildSel.draw(x+w-buildSel.w,y+(h-buildSel.h)/2);
	}
	else if(rodSel.pressed()){
		ofSetColor(255,255,255);
		rodInst.draw(x,y,w,h);
		ofSetColor(229,224,15);
		resetBut.draw(x+w-resetBut.w-20,y+h-resetBut.h-20);
	}
	else if(buildSel.pressed()){
		ofSetColor(255,255,255);
		buildInst.draw(x,y,w,h);
		ofSetColor(229,224,15);
		resetBut.draw(x+w-resetBut.w-20,y+h-resetBut.h-20);
	}
}
	
void instructionBox::update()
{

}
	
bool instructionBox::clickDown(int x, int y)
{
	bool ret=0;
	if(!rodSel.pressed()&&!buildSel.pressed()){
		rodSel.clickDown(x,y);
		buildSel.clickDown(x,y);
	}
	if(rodSel.pressed()||buildSel.pressed()){
		if(resetBut.clickDown(x,y)){
			reset();
		}
	}
	return ret;
}
	
bool instructionBox::clickUp()
{
	resetBut.clickUp();
	return false;
}
	
void instructionBox::reset()
{
	rodSel.setPressed(false);
	buildSel.setPressed(false);
}

int instructionBox::getState()
{
	int ret=0;
	if(rodSel.pressed()) ret=1;
	if(buildSel.pressed()) ret=2;
	return ret;
}

//--------------------- interface ----------

// no special needs in creating the interface

tableInterface::tableInterface()
{

}

tableInterface::~tableInterface()
{

}

void tableInterface::setup(motionTable * tbl)
{
	table=tbl;
	int numAcc=DIR.listDir("accelerogram/");
	double maxNet=0;
	for(int i=0; i<numAcc; i++){
		shakes.push_back(shakeTraj());
		shakes[i].setup(DIR.getPath(i));
		maxNet=max(shakes[i].maxNet,maxNet);
	}
	for(unsigned int i=0; i<shakes.size(); i++){
		//shakes[i].updateMaxNet(maxNet);
	}
	sinWave.setup(.5,10000,5);

	slide.setup(40,40,0);
	amp.setup(40,40,0);

	readout.setup(150,4);
	ampDisp.setup(150,4);

	ampUp.setup(32,32,"images/plus.png");
	ampDown.setup(32,32,"images/minus.png");

	freqBUp.setup(32,32,"images/plus.png");
	freqBDown.setup(32,32,"images/minus.png");

	buttons.setup(&shakes);
	sine.setup(&sinWave);
	stop.setup("RESET TABLE","fonts/DinC.ttf",40);
	stop.arial.setMode(OF_FONT_TOP);

	qSelect.setup("TRY AN EARTHQUAKE","fonts/DinC.ttf",30);
	qSelect.arial.setMode(OF_FONT_TOP);

	oSelect.setup("NEW USER?","fonts/DinC.ttf",30);
	oSelect.arial.setMode(OF_FONT_TOP);

	label.loadFont("fonts/DinC.ttf");
	label.setSize(30);
	label.setMode(OF_FONT_TOP);
	

	subtext.loadFont("fonts/Arial.ttf");
	subtext.setSize(15);
	subtext.setMode(OF_FONT_BOT);
	subtext.setMode(OF_FONT_CENTER);

	bQuaking=bWaving=false;

	amp.w=400;
	amp.setPercent(sinWave.amplPercent);

	homeTimer.set(1);
	homeTimer.pause();

	inst.setup();
	oSelect.setPressed(true);
}
	
void tableInterface::draw(int x, int y)
{
	int h=ofGetHeight()-(y-60)*2;
	int w=ofGetWidth()-(x-60)*2;
	int boxW=ofGetWidth()-x*2;
	int boxH=x*2;

	ofSetColor(255*.2,255*.2,255*.2,255*.75);
	ofRoundShape(x-60,y-60,w,h,20,true);
	ofSetColor(0,0,0);
	ofSetLineWidth(2.0);
	ofRoundShape(x-60,y-60,w,h,20,false);
	ofSetLineWidth(1.0);

	//--------  Background
	ofSetColor(150,150,150);
	ofRect(x-10,y-10,boxW+20,boxH+20);
	ofSetColor(0,0,0);
	ofRect(x-2,y-2,boxW+4,boxH+4);
	ofSetColor(255,255,255);
	ofRect(x,y,boxW,boxH);

	bool drawn=false;
	for(unsigned int i=0; i<shakes.size()&&!drawn; i++){
		if(shakes[i].isRunning()){
			drawn=true;
			shakes[i].draw(x,y,boxW,boxH);
			ofRectangle box=subtext.getBoundingBox(ssprintf("%i seconds",int(shakes[i].duration())),x+boxW-50,y+boxH+75);

			ofSetColor(0xe5e00f);
			ofTriangle(x+boxW,y+boxH,box.x-20+(box.height+40)/16,box.y-16,box.x+box.width+20-(box.height+40)/16,box.y-16);
			ofRoundShape(box.x-20,box.y-20,box.width+40,box.height+40,(box.height+40)/4,true);
			ofSetColor(0x333333);
			ofTriangle(x+boxW-3,y+boxH+5,box.x-15+(box.height+30)/16,box.y-12,box.x+box.width+15-(box.height+30)/16,box.y-12);
			ofRoundShape(box.x-15,box.y-15,box.width+30,box.height+30,(box.height+30)/4,true);
			ofSetColor(229,224,15);
			subtext.setMode(OF_FONT_TOP);
			subtext.drawString(ssprintf("%i seconds",int(shakes[i].duration())),box.x+box.width/2,box.y);
			subtext.setMode(OF_FONT_BOT);
		}
	}
	if(!drawn&&sine.sine->isRunning()) sine.sine->draw(x,y,boxW,boxH);

	ofShade(x,y,20,boxH,OF_RIGHT,.7);
	ofShade(ofGetWidth()-x,y,20,boxH,OF_LEFT,.7);

	if(table->isRunning()&&(ofGetElapsedTimeMillis()/500)%2){
		ofSetColor(251,176,23);
		ofRoundShape(stop.x-10,stop.y-10,stop.w+20,stop.h+20,(stop.h+20)/4,true);
	}

	if(qSelect.pressed()){
		ofSetColor(229,224,15);
		oSelect.draw((ofGetWidth()-oSelect.w)/2,y+h-(oSelect.h*2+50));
		drawQuakes(x+40,y+boxH+50,boxW/2,h-(boxH+250));
	}

	if(oSelect.pressed()){
		ofSetColor(229,224,15);
		if(inst.getState()){
			label.setSize(24);
			label.setMode(OF_FONT_BOT);
			label.drawString("Or",qSelect.x-label.stringWidth("Or")-20, qSelect.y+qSelect.h-10);
			label.setMode(OF_FONT_TOP);
			label.setSize(30);
			qSelect.draw(ofGetWidth()/2+40+(boxW/2-qSelect.w)/2,y+h-(qSelect.h*2+50));
		}
		inst.draw(ofGetWidth()/2+40,y+boxH+50,boxW/2,h-(boxH+250));
		drawFreq(x-40,y+boxH+50,boxW/2,h-(boxH+250));
	}

	if(table->isHoming()){
		ofSetColor(0,0,0,128);
		ofRect(0,0,ofGetWidth(),ofGetHeight());
	}
}

void tableInterface::drawGroupBox(int x, int y, int w, int h, string title,bool gray)
{
	ofColor k=ofGetStyle().color;

	int strWid=label.stringHeight(title)*1.5;
	 

	//------- white border box;
	ofNoFill();
	ofSetLineWidth(2.0);
	ofSetColor(255,255,255,k.a);
	ofRect(x+strWid,y,w-strWid,h);
	ofBeginShape();
	ofVertex(x+strWid,y+h);
	ofVertex(x,y+h);
	ofVertex(x,y+strWid);
	ofVertex(x+strWid,y);
	ofEndShape();
	ofSetLineWidth(1);
	ofFill();


	//------- rotated label for group
	ofSetColor(229,224,15,k.a);
	ofPushMatrix();
	ofTranslate(x+strWid/8,y+h-20);
	ofRotate(-90);
	label.drawString(title,0,0);
	ofPopMatrix();
}

void drawSlideGroup(int x, int y, int w, int h,string label, ofSlider & sld, ofButton & up, ofButton & dn,digitDisplay & disp, ofFont & fnt,double alph){
	ofSetColor(255,255,255,255*alph);
	fnt.drawString(label,x+w/2,disp.y-40);
	ofSetColor(0,0,0,255*alph);
	ofLine(x+10,disp.y-20,x+w-10,disp.y-20);

	ofSetColor(109,202,208,255*alph);
	sld.draw(x+(w-sld.w)/2-100,y,400,15);

	ofSetColor(255,255,255,255*alph);
	dn.draw(sld.x-dn.w-20,sld.y+(sld.h-dn.h)/2);
	up.draw(sld.x+sld.w+20,sld.y+(sld.h-up.h)/2);
}

void drawDisp(ofSlider & sld,digitDisplay & disp,string contents,string add, ofFont & fnt, double alph){
	ofRoundShadow(disp.x-10,disp.y-10,disp.w+20,disp.h+20,5,1*alph);
	ofSetColor(109,202,208,255*alph);
	disp.draw(contents,sld.x+sld.w+70,sld.y+(sld.h-disp.h)/2);
	fnt.setSize(25);
	fnt.drawString(add,disp.x+disp.w+30,disp.y+disp.h);
	fnt.setSize(15);
}

void tableInterface::drawFreq(int x, int y, int w, int h)
{
	double alph=1;

	double sideLabelW=label.stringHeight("Kjg")*1.5;

	//ofSetColor(64,128,200);
	ofSetColor(229,224,15,255*alph);
	sine.draw(x+(w-sine.w)/2,y+125);

	string title="FIND THE RESONANCE\n OF YOUR BUILDING";
	ofSetColor(229,224,15,255*alph);
	label.setSize(24);
	int wid=label.stringWidth(title);
	label.drawString(title,x+sideLabelW+10,y+10);
	label.setSize(30);

	ofSetColor(0,0,0,255*alph);
	ofSetLineWidth(2.0);
	ofLine(x+sideLabelW+10,y+10+label.stringHeight(title),x+w-10,y+10+label.stringHeight(title));
	ofSetLineWidth(1.0);


	string freqLabel="Frequency slider";
	int freq=sinWave.freq;
	drawDisp(slide,readout,ssprintf("%02i.%i",freq,int((sinWave.freq-freq)*10)),"hz",subtext,alph);
	drawSlideGroup(x+sideLabelW,y+275,w-sideLabelW,h,freqLabel,slide,freqBUp,freqBDown,readout,subtext,alph);

	string ampLabel="Amplitude slider";
	drawDisp(amp,ampDisp,ssprintf("%03i",int((sinWave.amplPercent)*100)),"%",subtext,alph);
	drawSlideGroup(x+sideLabelW,y+450,w-sideLabelW,h,ampLabel,amp,ampUp,ampDown,ampDisp,subtext,alph);


	ofSetColor(255,255,255,255*alph);
	drawGroupBox(x,y,w,h,"MANUAL CONTROL",bQuaking);
}

void tableInterface::drawQuakes(int x, int y, int w, int h)
{
	double alph=1;

	string title="SELECT AN EARTHQUAKE\nTO TEST YOUR STRUCTURE";
	ofSetColor(229,224,15,255*alph);
	label.setSize(24);
	int wid=label.stringWidth(title);
	label.drawString(title,x+label.stringHeight("Kjg")*1.5+10,y+10);
	label.setSize(30);

	ofSetColor(0,0,0,255*alph);
	ofSetLineWidth(2.0);
	ofLine(x+label.stringHeight("Kjg")*1.5+10,y+10+label.stringHeight(title),x+w-10,y+10+label.stringHeight(title));
	ofSetLineWidth(1.0);
	ofSetColor(255,255,255,255*alph);
	buttons.draw(x+w-buttons.w,y+h-(buttons.h+100));

	ofSetColor(255,255,255,255*alph);
	drawGroupBox(x,y,w,h,"EARTHQUAKES",bWaving);
}

void tableInterface::freqUp(double amt)
{
	if(sinWave.freq<20) sinWave.freq+=amt*sinWave.freq;
	//cout << sinWave.freq << " is the frequency" << endl;
}

void tableInterface::freqDown(double amt)
{
	if(sinWave.freq>.5)sinWave.freq-=amt*sinWave.freq;
	//cout << sinWave.freq << " is the frequency" << endl;
}

void tableInterface::dTimeUp(double amt)
{
	sinWave.dTime+=amt;
}

void tableInterface::dTimeDown(double amt)
{
	if(sinWave.dTime>amt) sinWave.dTime-=amt;
}

void tableInterface::update(){

	sinWave.update();
	if(table->justStopped()){
		bWaving=bQuaking=false;
	}
	if(homeTimer.getPercent()>.5&&homeTimer.getPercent()<.6){
		//table->home();
	}
	table->update();
}

bool tableInterface::clickDown(int _x, int _y)
{
	bool ret=0;
	if(!oSelect.pressed()){
		if(oSelect.clickDown(_x,_y)){
			ret=true;
			//homeTimer.reset();
			//homeTimer.run();
			table->home();
			inst.reset();
			qSelect.setPressed(false);
		}
		if(buttons.clickDown(_x,_y)){
			ret=true;
			table->handleTrajectory(*(buttons.lastPressed().shake));
		}
	}
	if(!ret&&!qSelect.pressed()){
		if(inst.getState()&&qSelect.clickDown(_x,_y)){
			//homeTimer.reset();
			//homeTimer.run();
			table->home();
			oSelect.setPressed(false);
		}
		if(sine.over(_x,_y)){
			if(!(sine.sine->isRunning())){
				oSelect.setPressed(true);
				ret=true;
				table->handleTrajectory(*sine.sine);
			}
			else{
				table->stopTable();
				bWaving=bQuaking=false;
			}
			sine.setPressed(true);
		}
		else if(slide.clickDown(_x,_y)){
			ret=true;
			if(abs(prevFreqPerc-slide.getPercent())<.1)
				sinWave.freq=pow(2,slide.getPercent()*(5)-1);
			else {
				slide.setPercent((prevFreqPerc+slide.getPercent())/2);
				sinWave.freq=pow(2,slide.getPercent()*(5)-1);
			}
			prevFreqPerc=slide.getPercent();
		}
		else if(freqBUp.clickDown(_x,_y)&&sinWave.freq<=15.9){
			ret=true;
			slide.setPercent((log(sinWave.freq+.1)/log(2.)+1)/5);
			sinWave.freq=pow(2,slide.getPercent()*(5)-1);
			prevFreqPerc=slide.getPercent();
		}
		else if(freqBDown.clickDown(_x,_y)&&sinWave.freq>.5){
			ret=true;
			slide.setPercent((log(sinWave.freq-.1)/log(2.)+1)/5);
			sinWave.freq=pow(2,slide.getPercent()*(5)-1);
			prevFreqPerc=slide.getPercent();
		}
		else if(amp.clickDown(_x,_y)){
			ret=true;
			sinWave.amplPercent=.1+.9*amp.getPercent();
		}
		else if(ampUp.clickDown(_x,_y)&&amp.getPercent()<=.9){
			ret=true;
			amp.setPercent(amp.getPercent()+.1);
			sinWave.amplPercent=.1+.9*amp.getPercent();
		}
		else if(ampDown.clickDown(_x,_y)&&amp.getPercent()>=.1){
			ret=true;
			amp.setPercent(amp.getPercent()-.1);
			sinWave.amplPercent=.1+.9*amp.getPercent();
		}
		if(ret&&!(sine.sine->isRunning())){
			table->handleTrajectory(*sine.sine);
		}
		inst.clickDown(_x,_y);
	}
	if(stop.clickDown(_x,_y)){
		bWaving=bQuaking=false;
		table->stopTable();
		ofGetAppPtr()->draw();
		table->home();
	}
	return ret;
}

bool tableInterface::clickUp()
{
	bool ret=0;
	ampUp.clickUp();
	ampDown.clickUp();
	freqBUp.clickUp();
	freqBDown.clickUp();
	ret|=slide.clickUp();
	ret|=amp.clickUp();
	ret|=buttons.clickUp();
	ret|=sine.clickUp();
	ret|=stop.clickUp();
	return ret;
}

void tableInterface::drag(int x, int y){
	slide.drag(x,y);
	//sinWave.freq=pow(2,slide.getPercent()*(5)-1);
	if(abs(prevFreqPerc-slide.getPercent())<.1)
		sinWave.freq=pow(2,slide.getPercent()*(5)-1);
	else {
		slide.setPercent((4*prevFreqPerc+slide.getPercent())/5);
		sinWave.freq=pow(2,slide.getPercent()*(5)-1);
	}
	prevFreqPerc=slide.getPercent();
	amp.drag(x,y);
	sinWave.amplPercent=.1+.9*amp.getPercent();
}