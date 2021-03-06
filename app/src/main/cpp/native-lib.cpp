#include <jni.h>
#include <string>
#include <android/log.h>
#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>

#define TAG "tcomm"
#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)


#define MAXLINE 4096
static int sockfd;


//创建SOCKET
extern "C"
JNIEXPORT jboolean JNICALL
Java_com_example_chen_tcpapplication_MainActivity_createSocket(JNIEnv *env, jobject instance) {
    LOGV("createSocket...");
    if ((sockfd = socket(AF_INET,SOCK_STREAM,0))<0){
        LOGE("create socket error:%s(error:%d)\n",strerror(errno),errno);
        return JNI_FALSE;
    }
    return JNI_TRUE;

}


char* jstringTostring(JNIEnv *env, jstring jstr)
{
    char* rtn = NULL;
    jclass clsstring = env->FindClass("java/lang/String");
    jstring  strencode = env->NewStringUTF("utf-8");
    jmethodID  mid = env->GetMethodID(clsstring,"getBytes","(Ljava/lang/String;)[B");
    jbyteArray barr= (jbyteArray)env->CallObjectMethod(jstr, mid, strencode);
    jsize alen = env->GetArrayLength(barr);
    jbyte* ba = env->GetByteArrayElements(barr, JNI_FALSE);
    if (alen >0)
    {
        rtn = static_cast<char *>(malloc(alen + 1));

        memcpy(rtn,ba,alen);
        rtn[alen] = 0;
    }
    env->ReleaseByteArrayElements(barr,ba,0);
    return rtn;
}

extern "C"
JNIEXPORT jboolean JNICALL
Java_com_example_chen_tcpapplication_MainActivity_connectSocket(JNIEnv *env, jobject instance,
                                                                jstring ip_, jint port) {
    LOGV("connectSocket...");
    struct sockaddr_in servaddr;
    memset(&servaddr,0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    char *ipaddr = jstringTostring(env, ip_);
    if (inet_pton(AF_INET, ipaddr, &servaddr.sin_addr) <= 0) {
        LOGE("inet_pton error for %s\n", ip_);
        return JNI_FALSE;
    }
    LOGV("connect...");
    if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
        LOGE("connect error: %s(errno: %d)\n", strerror(errno), errno);
        return JNI_FALSE;
    }
    LOGV("connect succeed!!!");
    return JNI_TRUE;

}




extern "C"
JNIEXPORT jint JNICALL
Java_com_example_chen_tcpapplication_MainActivity_recvSocket(JNIEnv *env, jobject instance,
                                                             jbyteArray buffer_, jint offset,
                                                             jint count) {
    LOGV("recvSocket");
    if (!sockfd) {
        LOGE("socket is failed!");
        return 0;
    }
    jbyte buf[count - offset];
    int rec_len;
    if ((rec_len = (int) recv(sockfd, buf, sizeof(buf), 0)) == -1) {
        LOGE("recv error");
        return 0;
    }

    buf[rec_len] = '\0';
    env->SetByteArrayRegion( buffer_, offset, count, buf);
    return rec_len;
}


extern "C"
JNIEXPORT jboolean JNICALL
Java_com_example_chen_tcpapplication_MainActivity_sendSocket(JNIEnv *env, jobject instance,
                                                             jstring sendData_) {
    LOGV("sendSocket");
    if (!sockfd) {
        printf("socket is failed! \n");
        return JNI_FALSE;
    }
    char *data = jstringTostring(env, sendData_);
    fgets(data, 4096, stdin);
    if (send(sockfd, data, strlen(data), 0) < 0) {
        return JNI_FALSE;
    }
    return JNI_TRUE;
}

extern "C"
JNIEXPORT jboolean JNICALL
Java_com_example_chen_tcpapplication_MainActivity_closeSocket(JNIEnv *env, jobject instance) {
        LOGV("closeSocket");
        if (!sockfd)
        return JNI_FALSE;
//        close(sockfd);
         return JNI_TRUE;
}