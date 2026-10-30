#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <iostream>
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
    curl_easy_cleanup(handle);
    curl_global_cleanup();
    return public_ip;
}

uint16_t get_port(){

  int fd = socket(AF_INET,SOCK_DGRAM,0);
  struct sockaddr_in resived_data_from_client{};
  resived_data_from_client.sin_family = AF_INET;
  resived_data_from_client.sin_port = htons(0);
  resived_data_from_client.sin_addr.s_addr = INADDR_ANY;
  if (bind(fd,(struct sockaddr*)&resived_data_from_client,sizeof(resived_data_from_client))== -1){
    print("failed to bind \n ");perror("bind");return 1; 
  };

  struct sockaddr_in get_port_addr{};
  socklen_t len = sizeof(get_port_addr);
  if (getsockname(fd, (struct sockaddr*)&get_port_addr,&len)==-1){
    print("failed to get port \n");perror("getsockname");return 1;
  };

  close(fd);
  
  return ntohs(get_port_addr.sin_port);   
  
}

void hello_massage(uint16_t& peer_port,std::string& peer_public_ip,std::string& password){
  

  uint16_t my_port = get_port();
  std::string my_public_ip = get_public_ip();

  print(
        "=======***======= \n"
        "your public ip :"  + my_public_ip + "\n"
        "your port :" + std::to_string(my_port) + "\n"
        "=======***======= \n"
        );
  
  print("enter peer public ip");std::cin >> peer_public_ip;print("\n");
  print("enter peer port");std::cin >> peer_port;print("\n");
  print("=======max number of litters is 256======= \n");
  print("enter your password");std::cin >> password;print("\n");
  print("=======***=======");
  
}

int connect_and_verify_the_peer(int& fd,std::string& password,uint16_t& peer_port,std::string& peer_public_ip,struct sockaddr_in& peer_client){
  
  peer_client.sin_port = peer_port;
  peer_client.sin_family = AF_INET;
  inet_pton(AF_INET,peer_public_ip.c_str(),&peer_client.sin_addr.s_addr);

  

  std::string recived_password{};
  recived_password.resize(256);
  
  struct sockaddr_in recived_data_addr{};
  
  socklen_t recived_data_addr_len = sizeof(recived_data_addr);
  
  
  sendto(fd,password.c_str(),password.size(),0,(struct sockaddr*)&peer_client,sizeof(peer_client));
  print("send our password to check if he exist");

  
  recvfrom(fd,recived_password.data(),recived_password.size(),0, (struct sockaddr*)&recived_data_addr,&recived_data_addr_len);

  print("someone send data");
  
  if (recived_password == password){
    print("checking sucess you can start");
  }else{
    print("maybe he sent wrong password or some haker try to hack you");
  }
}



int main(){
  uint16_t peer_port;
  std::string peer_public_ip;
  std::string password;

  hello_massage(peer_port,peer_public_ip,password);

  int fd = socket(AF_INET,SOCK_DGRAM,0);
  if (fd == -1){
    print("failed to create a socket : " ); perror("socket");return-1;
  }

  print("creating socket is sucseed");
  
  struct sockaddr_in peer_client;

  connect_and_verify_the_peer(fd,password,peer_port,peer_public_ip,peer_client);

  

  
  
  
  


  

          
  
  
  return 0;
}
