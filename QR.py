# coding= gbk
#@Time : 2020/9/27 
#@Author : С����
#@File : QQɨ���¼QQȺ����.py
#@Software: 
import requests
from PIL import Image
import threading
import psutil
import time
import re
import sys

def bkn(Skey):
    t = 5381
    n = 0
    o = len(Skey)
    while n < o:
        t += (t << 5) + ord(Skey[n])
        n += 1
    return t & 2147483647

def ptqrtoken(qrsig):
    n = len(qrsig)
    i = 0
    e = 0
    while n > i:
        e += (e << 5) + ord(qrsig[i])
        i += 1
    return 2147483647 & e

def QR():
    url = 'https://ssl.ptlogin2.qq.com/ptqrshow?appid=715030901&e=2&l=M&s=3&d=72&v=4&t=0.'+str(time.time())+'&daid=73&pt_3rd_aid=0'
    r = requests.get(url)
    ##ptqrlog = requests.utils.dict_from_cookiejar(r.cookies).get('qrsig')
    qrsig = requests.utils.dict_from_cookiejar(r.cookies).get('qrsig')
    with open(r'QR.png','wb') as f:
        f.write(r.content)
    f.close()
    print('��¼��ά���ȡ�ɹ�',time.strftime('%Y-%m-%d %H:%M:%S'))
    print('������ʾ��ά���Ƿ���Ч��ɨ�����ر�ͼƬ')
    return qrsig

def cookies(qrsig,ptqrtoken):
    while 1:
        url = 'https://ssl.ptlogin2.qq.com/ptqrlogin?u1=https%3A%2F%2Fqun.qq.com%2Fmanage.html%23click&ptqrtoken=' + str(ptqrtoken) + '&ptredirect=1&h=1&t=1&g=1&from_ui=1&ptlang=2052&action=0-0-' + str(time.time()) + '&js_ver=20032614&js_type=1&login_sig=&pt_uistyle=40&aid=715030901&daid=73&'
        cookies = {'qrsig': qrsig}
        r = requests.get(url,cookies = cookies)
        r1 = r.text 
        if '��ά��δʧЧ' in r1:
            print('��ά��δʧЧ',time.strftime('%Y-%m-%d %H:%M:%S'),end = "\r")
        elif '��ά����֤��' in r1:
            print('��ά����֤��',time.strftime('%Y-%m-%d %H:%M:%S'),end = "\r")
        elif '��ά����ʧЧ' in r1:
            print('��ά����ʧЧ',time.strftime('%Y-%m-%d %H:%M:%S'),end = "\r")
        else:
            print('��¼�ɹ�',time.strftime('%Y-%m-%d %H:%M:%S'))
            cookies = requests.utils.dict_from_cookiejar(r.cookies)
            uin = requests.utils.dict_from_cookiejar(r.cookies).get('uin')
            regex = re.compile(r'ptsigx=(.*?)&')
            sigx = re.findall(regex,r.text)[0]
            url = 'https://ptlogin2.qun.qq.com/check_sig?pttype=1&uin=' + uin + '&service=ptqrlogin&nodirect=0&ptsigx=' + sigx + '&s_url=https%3A%2F%2Fqun.qq.com%2Fmanage.html&f_url=&ptlang=2052&ptredirect=101&aid=715030901&daid=73&j_later=0&low_login_hour=0&regmaster=0&pt_login_type=3&pt_aid=0&pt_aaid=16&pt_light=0&pt_3rd_aid=0'
            r2 = requests.get(url,cookies=cookies,allow_redirects=False)
            targetCookies = requests.utils.dict_from_cookiejar(r2.cookies)
            skey = requests.utils.dict_from_cookiejar(r2.cookies).get('skey')
            for proc in psutil.process_iter():
                if not proc in process_list:
                    proc.kill()
            break
        time.sleep(3)
    return targetCookies,skey

def qun(cookies,bkn,num):
    url = 'https://qun.qq.com/cgi-bin/qun_mgr/get_group_list'
    data = {'bkn':bkn}
    cookies = cookies
    r = requests.post(url,data = data,cookies = cookies)
    regex = re.compile(r'"gc":(\d+),"gn')
    r = re.findall(regex,r.text)
    if num in r:
        return True
    else:
        return False

class Thread(threading.Thread):
	def run(self):
		pngOpen()
		
def pngOpen():
	im = Image.open(r'QR.png')
	im = im.resize((350,350))
	im.show()
	return 
    
def start():
    qrsig = QR()
    global ptqrtoken 
    global bkn
    ptqrtoken = ptqrtoken(qrsig)
    global process_list
    process_list = []
    for proc in psutil.process_iter():
        process_list.append(proc)
    thread1 = Thread()
    thread1.start()
    cookie = cookies(qrsig,ptqrtoken)
    skey = cookie[1]
    bkn = bkn(skey)
    ck = cookie[0]
    state = qun(ck, bkn,'1028201286')
    if state:
        print('��ϲ�㣬��֤�ɹ�~')
        #print('����ִ����֤�ɹ���Ĵ���')
        time.sleep(3)
        return 0
    else:
        #print(skey)
        ##print(bkn)
        print('���ź�����֤ʧ��~')
        print('������3����˳�...')
        time.sleep(3)
        return 1

def main():
    out = start()
    if out == 0:
        sys.exit(0)#�˴����˳�ֵ����Ϊ����������C++�����ȡ�����˳�ֵ���ܻ����
    else:
        if out==1:
            sys.exit(1)
        else:
            sys.exit(2)

if __name__ == '__main__':
    main()