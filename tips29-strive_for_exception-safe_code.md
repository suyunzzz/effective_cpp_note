# Tips 29: strive for exception-safe code

> 本文将从异常安全问题、异常安全的解决来进行介绍

[TOC]

假设我们有一个`PrettyMenu`的对象，需要在多线程环境中调用修改其背景图片：

```cpp
class PrettyMenu {
public:
  ...
  void changeBackground(std::istream& imgSrc); // change background image
private:
  ...
  Mutex mutex; // mutex for this object 
  Image *bgImage; // current background image
  int imageChanges; // # of times image has been changed
};
```

对于`changeBackground`一个可能的实现如下：

```cpp
void PrettyMenu::changeBackground(std::istream& imgSrc){
  _mutex.lock();
  delete bgImage;
  ++imageChanges;
  bgImage = new Image(imgSrc);
  _mutex.unlock();
}
```

这个函数没有满足异常安全的两个条件：
1. 当异常抛时，带有异常安全的函数会**不泄露任何资源**

> 一旦在`new Image`时异常， 那么_mutex永远不会释放

2. 当异常抛时，带有异常安全的函数**不允许数据的败坏**

> 如果`new bgImage`就会指向一个已经删除的对象，且`imageChanges`已经被累加

## 资源泄漏

> 针对第一个问题（泄露资源），可以使用RAII, 这里使用lock_guard

```cpp
void PrettyMenu::changeBackground(std::istream& imgSrc){
  std::lock_guard<std::mutex> lg(_mutex);
  delete bgImage;
  ++imageChanges;
  bgImage = new Image(imgSrc);
}
```

## 数据败坏

### 异常安全函数

- 基本承诺

> 如果抛出异常，绝不败坏数据资源，但是具体状态不可预知。如例子中的`new Image`抛出异常后，背景图片的状态可能时默认图片也可能时原始图片，这时不可预知的



- 强烈保证

> 如果抛出异常，那么程序状态不改变。也就是调用函数如果失败，那么会保持调用之前的状态


- 不抛出保证

> no-throw，不常用，因为只要有动态内存的分配就有可能抛出异常(STL等)

---

针对数据败坏的问题，大多数时候，我们都是在**基本承诺**和**强烈保证**之间抉择.


首先，我们为`changeBackground`提供强烈保证，只需要使用智能指针以保证在内存分配成功后才对bgImage进行修改：

```cpp
void PrettyMenu::changeBackground(std::istream& imgSrc){
  std::lock_guard<std::mutex> lg(_mutex);
  // 两种方式都可以
  bgImage.reset(new Image(imgSrc));
  // bgImage = std::make_shared<Image>(Image(imgSrc));  
  ++imageChanges;
}
```


### pimpl idiom
事实上，如果`input stream`在构造Image前已经被搬走，那么上述实现中Image的构造函数抛出异常，所以上述实现在解决这个问题之前只能提供**基本承诺**，解决方式就是**copy and swap策略**

实现上通过将所有“隶属原对象的数据”， 从原对象放进另一个对象里，然后在原对象中通过指针指向这个所谓的实现对象(implementation object，即副本)。这种手法通常被称为***pimpl idiom***, 条款31详细介绍。对于`PrettyMenu`而言，典型写法如下:


```cpp
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
```

## summary

1. 异常安全函数即使发生异常也不会泄露资源或者允许任何数据结构败坏。这样的函数区分为三种可能的保证：基本型、强烈型、不抛出异常型
2. “强烈保证”往往可以通过swap-and-copy实现出来，但是会有资源拷贝，因此并不是所有场景都要实现异常安全的“强烈保证”
3. 函数提供的异常安全保证通过最高只等于其调用的各个函数的“异常安全保证的最弱者”（木桶效应）

