#include<iostream>
#include<string>
#include<string.h>
#include<curl\curl.h>
#include <fstream>
using namespace std;

struct curl_slist* cookies = NULL;

FILE* fp;

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
size_t write_data(void* ptr, size_t size, size_t nmemb, void* stream)
{
	int written = fwrite(ptr, size, nmemb, (FILE*)fp);
	return written;
}
//http GET请求  
CURLcode curl_get_req(const std::string& url, std::string& response)
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
		header_list = curl_slist_append(header_list, "User-Agent: Mozilla/5.0 (Windows NT 10.0; WOW64; Trident/7.0; rv:11.0) like Gecko");
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header_list);

		//不接收响应头数据0代表不接收 1代表接收
		curl_easy_setopt(curl, CURLOPT_HEADER, 0);

		//设置请求的URL地址 
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

		//设置ssl验证
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);

		//文件形式输出cookie
		curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "");

		//CURLOPT_VERBOSE的值为1时，会显示详细的调试信息
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 0);

		curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);

		//设置数据接收函数
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, req_reply);
		//curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&response);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);

		curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);

		//设置超时时间
		curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 6); // set transport and time out time  
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 6);
		// 开启请求  
		res = curl_easy_perform(curl);
		//curl_easy_getinfo(curl, CURLINFO_COOKIELIST, &cookies);
		//printf("Cookies:%s\n", cookies->data);//原Cookie
	}
	// 释放curl 
	curl_easy_cleanup(curl);

	return res;
}
void get_cookie() {
	char* qr = "qrsig";
	char* result;
	char* cookie = cookies->data;
	result = strstr(cookie, qr);
	result += sizeof("qrsig");
	//printf("%s\n", result);
}
std::string get_QR(std::string res) {
	/*char* qr = "SameSite = None;";
	char* result;
	char* cookie = (char*)res.c_str();
	result = strstr(cookie,qr);*/
	char* result = (char*)res.c_str();
	result += sizeof("�PNG");
	printf("%s\n", result);
	return result;
}
void writePng(const std::string& szWritePath, std::string& szContent, const int& ilength) {
	FILE* pw;
	pw = std::fopen(szWritePath.c_str(), "w");
	char* pCache = new char[ilength];
	memcpy(pCache, szContent.c_str(), ilength * sizeof(char));
	fwrite(pCache, sizeof(char), ilength, pw);
	fflush(pw);
	fclose(pw);
	if (nullptr != pCache)
	{
		delete[] pCache;
		pCache = nullptr;
	}
}
std::string ReadPng(std::string path) {
	ifstream is(path.c_str(), ifstream::in);

	// 计算图片长度
	is.seekg(0, is.end);
	int length = is.tellg();
	std::cout << length << std::endl;
	char* buffer = new char[length];

	// 读取图片
	is.seekg(0, is.beg);
	is.read(buffer, length);
	for (int i = 0; i < length; i++) {
		std::cout << (int)(unsigned char)buffer[i] << " ";
		if ((i + 1) % 20 == 0)
			std::cout << std::endl;
	}
	return path;
}

int main1()
{
	std::string file = "QR.PNG";
	fp = fopen(file.c_str(), "wb");
	std::string szWritePath = "QR.txt";
	system("chcp 65001");
	system("cls");
	//string getUrlStr = "https://www.baidu.com/";
	//string getUrlStr = "https://api.yanyuwangluo.cn/api/ACG/api.php";
	string getUrlStr = "https://tva4.sinaimg.cn/large/0072Vf1pgy1foxli0u4b3j31hc0u0qoj.jpg";
	string getResponseStr;
	auto res = curl_get_req(getUrlStr, getResponseStr);
	//printf("%s\n", getUrlStr);
	if (res == CURLE_OK)
	{
		cout << getResponseStr << endl;
	}
	//get_cookie();
	//ReadPng("1.jpg");

	//get_QR(getResponseStr);
	writePng(szWritePath, getResponseStr, sizeof(getResponseStr));
	cout << "End" << endl;
	getchar();
	return 0;
}