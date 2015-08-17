#include "mbed.h"
#include <stdio.h>
#include <string>
#include <SerialBase.h>

string mHostName="agri-kuc.appspot.com";
string mParseRestKey="your-rest-key";
string mParseAppKey ="your-app-key";

Serial mPc(USBTX, USBRX);
Timer mTimer;

AnalogIn inSen1(dp13);
AnalogIn inSen2(dp11);
AnalogIn inSen3(dp10);
AnalogIn inSen4(dp9 );

DigitalOut outRelary_1(dp14);
DigitalOut outRelary_2(dp28);
DigitalOut outRelary_3(dp26);
DigitalOut outRelary_4(dp25);

int mSTAT =0;
int mSTAT_CONECT=1;
int mSTAT_VALVE=2;
int mTmMax=60;

int mValveGet = 20;
int mCtOpen   = 0;

int mOK_CODE=1;
int mNG_CODE=0;
int mWait_Debug=5;

string mResponse3="";
string mOK_STR="OK";
string mKey_HEAD ="web-response1=";

//Struct
struct stParam{
 string stat;
 string statCode;
 string moi_num;
 string vnum_1;
 string vnum_2;
 string vnum_3;
 string vnum_4;
 string kai_num_1;
 string kai_num_2;
 string kai_num_3;
 string kai_num_4;
};

string conv_zeroStr(string src ,int width){
    string ret="00000000";
    ret +=src;
    int iNum = ret.length();
    ret = ret.substr(iNum-width);
    return ret;    
}
/*
wait_forKey
wCount : count (10 msec * wCount= Limit wait-time)
*/
bool wait_forKey(  string sKey, int wCount){
    bool ret=false;   
    
    //int iCt=0;
    string sRes="";
    
    if(mPc.readable()){
        while(1) {
            char c= mPc.getc();
            sRes +=c;
            int loc = sRes.find( sKey, 0 );
            if(loc != string::npos ){
                if(loc >= 0){
                    //mPc.printf("#find-loc= %d\n" , loc);
                    return true;
                }
            }
        }
    }
    return ret;
}

//
string get_message(){
    int iSen  =0;
    string ret="";
    
    float fSen  = inSen1;
//printf("fSen= %f\n" ,fSen);
    iSen  = int(fSen  * 1000);
    
    char buff[]="0000";
    sprintf(buff, "%d", iSen);
    ret="GET /trans.php?mc_id=1&rkey="+mParseRestKey;
    ret +="&apkey="+mParseAppKey;
    ret +="&snum_1=";
    ret += conv_zeroStr(buff ,4);
    ret +="&snum_2=0000&snum_3=0000&snum_4=0000 HTTP/1.1\r\n";
    ret +="Host: " +mHostName+ "\r\n\r\n";
    return ret;
}

//main
int main() {
    mPc.baud( 115200 );
    mSTAT = mSTAT_CONECT;
    mTimer.start();
    
    while(1) {
        float sec= mTimer.read();
        if(mSTAT == mSTAT_CONECT){
            if(sec >= mTmMax){
                mTimer.reset();
                string msg=get_message();
                int len=msg.length();
                mPc.printf("AT+CIPSTART=\"TCP\",\"%s\",80\r\n" , mHostName);
                wait(mWait_Debug);
                mPc.printf("AT+CIPSEND=%d\r\n" , len);
                wait(mWait_Debug);
                mPc.printf("%s",msg);
                wait(mWait_Debug);
                mPc.printf("AT+CIPCLOSE\r\n");
                wait(1);
            }
        }
    }
}
