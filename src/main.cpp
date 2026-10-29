#include <cstdio>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <curl/curl.h>
#include <string>

#include <fcntl.h>
#include <unistd.h>

void print(const std::string outpot){
  write(1,outpot.c_str(),outpot.size());
}

int write_call_back(void *ptr, size_t size, size_t nmemb, void *userdata){
    std::string* res = static_cast<std::string*>(userdata);
    res->append(static_cast<char*>(ptr),size*nmemb);
    return  size * nmemb;
}

std::string get_public_ip(){
    std::string public_ip{};
    curl_global_init(CURL_GLOBAL_DEFAULT);
    CURL *handle =curl_easy_init();
    curl_easy_setopt(handle,CURLOPT_URL,"https://api.ipify.org");
    curl_easy_setopt(handle,CURLOPT_WRITEFUNCTION,write_call_back);
    curl_easy_setopt(handle,CURLOPT_WRITEDATA,&public_ip);
    curl_easy_perform(handle);
    curl_global_cleanup();
    return public_ip;
}

std::string get_port(){

  int fd = socket(AF_INET,SOCK_DGRAM,0);
  struct sockaddr_in resived_data_from_client{};
  resived_data_from_client.sin_family = AF_INET;
  resived_data_from_client.sin_port = htons(0);
  resived_data_from_client.sin_addr.s_addr = INADDR_ANY;
  if (bind(fd,(struct sockaddr*)&resived_data_from_client,sizeof(resived_data_from_client))== "-1"){
    print("failed to bind \n ");perror("bind");return "-1"; 
  };

  struct sockaddr_in get_port_addr{};
  socklen_t len = sizeof(get_port_addr);
  if (getsockname(fd, (struct sockaddr*)&get_port_addr,&len)==-1){
    print("failed to get port \n");perror("getsockname");return "-1";
  };
  
  return std::to_string(ntohs(get_port_addr.sin_port));   
  
}


int main(){

  print(
        "public ip : "+get_public_ip() +"\n"
        "port : " + get_port() + "\n"
        );
          
  
  
  return 0;
}
