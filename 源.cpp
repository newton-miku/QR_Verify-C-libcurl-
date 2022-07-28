#include <iostream>
#include <string>
#include <curl\curl.h>
#include "encode.h"
#define SEP  "\t"
#define GET_TIME std::to_string(std::time(0))
using namespace std;

struct curl_slist* cookies = NULL;//返回的cookies
long httpcode;//网页状态码
FILE* fp;//QR文件指针

std::string getResponseStr;
std::string qrsig;
std::string skey;
std::string sigx;
std::string uin;
std::string pt4_token;
std::string p_skey;
std::string bkn;
std::string ptqrtoken;

#define GET_PNG		0
#define GET_JSON	1

char* getCurrentTime(int out);
void set_qrsig();

//get请求和post请求数据响应函数
size_t req_reply(void* ptr, size_t size, size_t nmemb, void* stream)
{
	//在注释的里面可以打印请求流，cookie的信息
	//cout << "----->reply" << endl;
	string* str = (string*)stream;
	//cout << *str << endl;
	(*str).append((char*)ptr, size * nmemb);
	return size * nmemb;
}
//写入文件函数
size_t write_data(void* ptr, size_t size, size_t nmemb, void* stream)
{
	int written = fwrite(ptr, size, nmemb, (FILE*)fp);
	return written;
}
void PrintCK() {
	while (cookies) {
		cout << cookies->data << endl;
		cookies = cookies->next;
	}
}

//http GET请求  
CURLcode curl_get_req(const std::string& url, std::string& response,int type)
{
	//curl初始化  
	CURL* curl = curl_easy_init();
	// curl返回值 
	CURLcode res;
	if (curl)
	{
		//设置curl的请求头
		struct curl_slist* header_list = NULL;
		std::string cookieFile = "C:\\Users\\13165\\Desktop\\cookie.txt";
		//header_list = curl_slist_append(header_list, "User-Agent: Mozilla/5.0 (Windows NT 10.0; WOW64; Trident/7.0; rv:11.0) like Gecko");
		header_list = curl_slist_append(header_list, "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/103.0.0.0 Safari/537.36");
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header_list);

		//不接收响应头数据0代表不接收 1代表接收
		curl_easy_setopt(curl, CURLOPT_HEADER, 0);

		//设置请求的URL地址 
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

		//设置ssl验证
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);
		
		//文件形式输出cookie
		curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "");//启动cookie引擎

		//CURLOPT_VERBOSE的值为1时，会显示详细的调试信息
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 0);

		curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);

		//设置数据接收函数
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, req_reply);
		switch (type) {
		case GET_PNG:
			fp = fopen(response.c_str(), "wb");
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data); 
			break;
		case GET_JSON:
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&response); break;
		}

		curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);

		//设置超时时间
		curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 6); // set transport and time out time  
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 6);

		// 开启请求  
		res = curl_easy_perform(curl);
		curl_easy_getinfo(curl, CURLINFO_COOKIELIST, &cookies);//保存cookie到字符串cookies中
		//printf("Cookies:%s\n", cookies->data);//原Cookie
	}
	// 释放curl 
	curl_easy_cleanup(curl);

	return res;
}
CURLcode curl_get_req(const std::string& url, std::string& response,int type,std::string cookie,int head = 0)
{
	//curl初始化  
	CURL* curl = curl_easy_init();
	// curl返回值 
	CURLcode res;
	if (curl)
	{
		//设置curl的请求头
		struct curl_slist* header_list = NULL;
		header_list = curl_slist_append(header_list, "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/103.0.0.0 Safari/537.36");
		std::string ck = "Cookie:" + cookie;
		header_list = curl_slist_append(header_list, ck.c_str());//请求头中的cookie
		cookies = NULL;

		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header_list);

		//接收响应头数据0代表不接收 1代表接收
		curl_easy_setopt(curl, CURLOPT_HEADER, head);

		//设置请求的URL地址 
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

		//获取网页请求状态码
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpcode);

		//允许几次重定向
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 0);

		//设置ssl验证
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, true);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, true);
		
		//设置cookie
		curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "");

		//CURLOPT_VERBOSE的值为1时，会显示详细的调试信息
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 0);

		curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);

		//设置数据接收函数
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, req_reply);
		switch (type) {
		case GET_PNG:
			fp = fopen(response.c_str(), "wb");
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data); 
			break;
		case GET_JSON:
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&response); break;
		}

		curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 0);

		//设置超时时间
		curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 6); // set transport and time out time  
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 6);

		// 开启请求  
		res = curl_easy_perform(curl);
		curl_easy_getinfo(curl, CURLINFO_COOKIELIST, &cookies);//保存cookie到字符串cookies中
	}
	// 释放curl 
	curl_easy_cleanup(curl);
	return res;
}

//http POST请求  
CURLcode curl_post_req(const string& url, const string& postParams, string& response,std::string cookie)
{
	// curl初始化  
	CURL* curl = curl_easy_init();
	// curl返回值 
	CURLcode res;
	if (curl)
	{
		// set params
		//设置curl的请求头
		struct curl_slist* header_list = NULL;
		header_list = curl_slist_append(header_list, "User-Agent: Mozilla/5.0 (Windows NT 10.0; WOW64; Trident/7.0; rv:11.0) like Gecko");
		header_list = curl_slist_append(header_list, "Content-Type:application/x-www-form-urlencoded; charset=UTF-8");
		std::string ck = "Cookie:" + cookie;
		header_list = curl_slist_append(header_list, ck.c_str());
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header_list);

		//不接收响应头数据0代表不接收 1代表接收
		curl_easy_setopt(curl, CURLOPT_HEADER, 0);

		//设置请求为post请求
		curl_easy_setopt(curl, CURLOPT_POST, 1);

		//设置请求的URL地址
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		//设置post请求的参数
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postParams.c_str());

		//设置ssl验证
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);

		//CURLOPT_VERBOSE的值为1时，会显示详细的调试信息
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 0);

		curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);

		//设置数据接收和写入函数
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, req_reply);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&response);

		curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);

		//设置超时时间
		curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 6);
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 6);

		// 开启post请求
		res = curl_easy_perform(curl);
	}
	//释放curl 
	curl_easy_cleanup(curl);
	return res;
}

//bkn的算法
int set_bkn() {
	int t = 5381, n = 0, o = skey.length();
	while (n<o)
	{
		t += (t << 5) + toascii(skey.at(n));
		n++;
	}
	return (t & 2147483647);
}
//获取qrsig
void set_qrsig() {
	char* qr = "qrsig";
	char* result;
	char* cookie = cookies->data;
	result = strstr(cookie,qr);
	result += sizeof("qrsig");
	qrsig = result;
	//printf("%s\n", result);
}

//获取ptqrtoken
int set_ptqrtoken() {
	//qrsig = "enR8pbm6qeWQIu7C33CT*wpwg2upFruEL4lB6CB3wfaSacRtvHtAS2JbamaA1HdW";
	int len = qrsig.length(),i = 0,e=0;
	while (len > i) {
		e += (e << 5) + toascii(qrsig.at(i));
		i++;
	}
	return (2147483647 & e);
}
char* getSigx(std::string str) {
	char* sigxSign = "&ptsigx=",* result{};
	char* url = (char*)str.c_str(), * point;
	point = strstr(url, sigxSign);
	point += sizeof("&ptsigx");
	std::string a = point;
	for(int i=0;i<145;i++)
	{
		sigx += a.at(i);
	}
	return result;
}

//获取登陆后的url
char* getSkeyUrl(std::string str) {
	/*char* url = "https://ptlogin2.qun.qq.com/";
	char* skey = "skey";
	char* res = strstr((char*)str.c_str(), skey);
	cout << res;*/
	int i = 0;
	char* res = strtok((char*)str.c_str(), "','"); i++;
	while (res != NULL&&i!=4) {
		res = strtok(NULL, "','"); i++;
	}
	return res;
}
std::string getSkey() {
	int a = 0;
	int pt4_tokenBool = 0;
	while (cookies&&a<=4) {
		int i = 0;
		const char* res = strtok(cookies->data, "\t");
		i++;
		while (res != NULL && i != 6) {
			res = strtok(NULL, "\t"); i++;
		}
		if (stricmp(res, "skey")==0) {
			skey = res = strtok(NULL, "\t");
			a++;
		}
		else if (stricmp(res, "p_uin")==0 || stricmp(res, "uin") == 0) {
			res = strtok(NULL, "\t");
			uin = res;
			a++;
		}
		else if (stricmp(res, "pt4_token")==0 && pt4_tokenBool == 0)
		{
			res = strtok(NULL, "\t");
			pt4_token = res;
			pt4_tokenBool = 1;
			a++;
		}
		else if (stricmp(res, "p_skey")==0)
		{
			res = strtok(NULL, "\t");
			p_skey = res; a++;
		}
		else
		{
			cookies = cookies->next;
			continue;
		}
			cookies = cookies->next;
		
	}
	return skey;
}

//判断curl返回结果是否正确
bool curlRes(CURLcode res) {
	if (res == CURLE_OK)
	{
		return true;
	}
	else {
		printf("Curl perform failed: %s\n", curl_easy_strerror(res));
		return false;
	}
}
//发送心跳包获取当前状态
void heartPack() {
	while (1) {
		std::string url = "https://ssl.ptlogin2.qq.com/ptqrlogin?u1=https%3A%2F%2Fqun.qq.com%2Fmanage.html%23click&ptqrtoken=" + 
			ptqrtoken + "&ptredirect=1&h=1&t=1&g=1&from_ui=1&ptlang=2052&action=0-0-" + 
			GET_TIME + "&js_ver=20032614&js_type=1&login_sig=&pt_uistyle=40&aid=715030901&daid=73&";
		std::string getResponseStr;
		std::string my_cookie ="qrsig=" + qrsig;
		auto res = curl_get_req(url, getResponseStr, GET_JSON, my_cookie);
		getResponseStr = Utf8ToGb2312(getResponseStr.c_str());
		if (curlRes(res)) {
			if (strstr(getResponseStr.c_str(), "二维码未失效")) {//strstr
				printf("二维码未失效，%s\r", getCurrentTime(0));
			}
			else if (strstr(getResponseStr.c_str(), "二维码认证中"))
			{
				printf("二维码认证中，%s\r", getCurrentTime(0));
			}
			else if (strstr(getResponseStr.c_str(), "二维码已失效"))
			{
				printf("二维码已失效，%s\r", getCurrentTime(0));
			}
			else {
				printf("登录成功，%s\n", getCurrentTime(0));
				//PrintCK();
				getSkey();
				std::string loginUrl = getSkeyUrl(getResponseStr);
				getSigx(loginUrl);
				loginUrl = "https://ptlogin2.qun.qq.com/check_sig?pttype=1&uin=" + uin + "&service=ptqrlogin&nodirect=0&ptsigx=" + 
					sigx + "&s_url=https%3A%2F%2Fqun.qq.com%2Fmanage.html&f_url=&ptlang=2052&ptredirect=101&aid=715030901&daid=73&j_later=0&low_login_hour=0&regmaster=0&pt_login_type=3&pt_aid=0&pt_aaid=16&pt_light=0&pt_3rd_aid=0";
				res = curl_get_req(loginUrl, getResponseStr, GET_JSON, my_cookie,0);
				if (curlRes(res)) {
					skey = getSkey();
					bkn = to_string(set_bkn());//算出bkn
				}
				break;
			}
		}
		else
		{
			
			return;
		}
	}
}

bool findQun(std::string str) {
	const char* a = strstr(str.c_str(), "\"gc\":1028201286");//使用"gc":qq群号的串，避免因某个群主的qq号与目标群号相同而导致验证失误（虽然极小概率就是了）
	if (a == NULL) {
		return false;
	}
	else return true;
}

bool getQun() {
	std::string url = "https://qun.qq.com/cgi-bin/qun_mgr/get_group_list";
	string resPost0;
	std::string bknS = "bkn=" + bkn;
	std::string post_cookie = "skey=" + skey+";uin="+uin+ ";p_skey=" + p_skey+";p_uin="+uin+";pt4_token = "+ pt4_token;
	auto res = curl_post_req(url, bknS, resPost0,post_cookie);
	if (curlRes(res)) {
		//cout << resPost0 << endl;
		return findQun(resPost0);
	}
}



//获取当前时间并输出
char* getCurrentTime(int out=1) {
	time_t rawtime;
	struct tm* info;
	char buffer[80];

	time(&rawtime);
	info = localtime(&rawtime);
	strftime(buffer, 80, "%Y-%m-%d %H:%M:%S", info);//时间格式 YYYY-MM-DD HH:MM:SS
	if (out) {
		printf("%s", buffer);
	}
		return buffer;
}

int main()
{
	std::string QRPath = "QR.png";//QR文件位置
	//system("chcp 65001");
	system("cls");
	string getUrlStr = "https://ssl.ptlogin2.qq.com/ptqrshow?appid=715030901&e=2&l=M&s=3&d=72&v=4&t=0."+std::to_string(std::time(0))+"&daid=73&pt_3rd_aid=0";
	//printf("%s\n", getUrlStr);
	auto res = curl_get_req(getUrlStr, QRPath, GET_PNG);//获取二维码
	if (curlRes(res))
	{
		//cout << getResponseStr << endl;
		fclose(fp);
		printf("登录二维码获取成功，");getCurrentTime();//输出相应提示
		printf("\n");
		set_qrsig();
		ptqrtoken = to_string(set_ptqrtoken());
		heartPack();
		if (getQun())
			cout << "验证成功" << endl;
		else
			cout << "验证失败" << endl;
	}
	
	cout << "End" << endl;
	getchar();
	return 0;
}

