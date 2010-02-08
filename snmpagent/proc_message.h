#ifndef __PROC_MESSAGE_H__
#define __PROC_MESSAGE_H__


class    proc_message
{
private:
    
public:
    proc_message(){};
    virtual    ~proc_message(){};
    
    virtual    void    do_process() = 0;
};


#endif //__PROC_MESSAGE_H__
