
#ifndef __SINGLETON_H___
#define __SINGLETON_H___

namespace LOG
{
  template<class T>
  class Singleton
  {
    public:

    static T& getInstance(); // Instance is in here
    bool forceInstanceCreation();

    protected:

    Singleton();

    private:

    Singleton(const Singleton&);
    Singleton& operator=(const Singleton&);
  };

  template<class T>
  T& Singleton<T>::getInstance()
  {
    static T instance;
    return instance;
  }

  template<class T>
  bool Singleton<T>::forceInstanceCreation()
  {
    return true;
  }

  template<class T>
  Singleton<T>::Singleton()
  {
  }

  template<class T>
  Singleton<T>::Singleton(const Singleton&)
  {
  }

  template<class T>
  Singleton<T>& Singleton<T>::operator=(const Singleton&)
  {
    return *this;
  }

} // namespace log

#endif // __SINGLETON_H___
