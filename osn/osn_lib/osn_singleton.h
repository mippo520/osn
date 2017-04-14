//
//  osn_singleton.h
//  osn
//
//  Created by liqing on 17/4/5.
//  Copyright © 2017年 liqing. All rights reserved.
//

#ifndef osn_singleton_h
#define osn_singleton_h

template <typename T>
class OsnSingleton
{
protected:
    struct object_creator
    {
        object_creator()
        {
            OsnSingleton<T>::instance();
        }
        inline void do_nothing()const {};
    };
    static object_creator create_object_;
    
public:
    typedef T object_type;
    static object_type& instance()
    {
        static object_type instance_;
        //do_nothing 是必要的，do_nothing的作用有点意思，
        //如果不加create_object_.do_nothing();这句话，在main函数前面
        //create_object_的构造函数都不会被调用，instance当然也不会被调用，
        //可能是模版的延迟实现的特效导致，如果没有这句话，编译器也不会实现
        // Singleton<T>::object_creator,所以就会导致这个问题
        create_object_.do_nothing();
        return instance_;
    }
    
    void do_something();
};


//因为create_object_是类的静态变量，必须有一个通用的声明
template<typename T>
typename OsnSingleton<T>::object_creator OsnSingleton<T>::create_object_;

#endif /* osn_singleton_h */
