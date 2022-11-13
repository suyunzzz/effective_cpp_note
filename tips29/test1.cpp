#include "../common.h"

class Image{
public:
  Image(std::istream& imgSrc){}
private:
};

struct  PMImpl{
  int imageChanges;
  std::shared_ptr<Image> bgImage;
};

class PrettyMenu {
public:
  void changeBackground(std::istream& imgSrc); // change background image
private:
  std::mutex _mutex; // mutex for this object 
  std::shared_ptr<PMImpl> pImpl;
};

/*
void PrettyMenu::changeBackground(std::istream& imgSrc){
  _mutex.lock();
  delete bgImage;
  ++imageChanges;
  bgImage = new Image(imgSrc);
  _mutex.unlock();
}
*/

/*
void PrettyMenu::changeBackground(std::istream& imgSrc){
  std::lock_guard<std::mutex> lg(_mutex);
  delete bgImage;
  ++imageChanges;
  bgImage = new Image(imgSrc);
}
*/

/*
void PrettyMenu::changeBackground(std::istream& imgSrc){
  std::lock_guard<std::mutex> lg(_mutex);
  // 两种方式都可以
  bgImage.reset(new Image(imgSrc));
  // bgImage = std::make_shared<Image>(Image(imgSrc));  
  ++imageChanges;
}
*/

void PrettyMenu::changeBackground(std::istream& imgSrc){
  std::lock_guard<std::mutex> lg(_mutex);
  // 1. 创建副本
  std::shared_ptr<PMImpl> pImpl_new = std::make_shared<PMImpl>(*pImpl);

  // 2. 修改副本
  pImpl_new->bgImage = std::make_shared<Image>(imgSrc);
  // pImpl_new->bgImage.reset(new Image(imgSrc));
  pImpl_new->imageChanges++;

  // 3. swap
  std::swap(pImpl, pImpl_new); 
}



int main(){

    return 0;
}