#ifndef    __SNMPBRIDGE_H__
#define    __SNMPBRIDGE_H__
namespace    l7vs{
class    snmpbridge{
public:

//variable
    static    bool    initialize_called;
    static    bool    initialize_fail;

    static    bool    finalize_called;

//function
    //! constractor
    //! @param[in]
    //! @param[in]
    snmpbridge( l7vsd& l7vsd_in, boost::asio::io_service& io_service_in )
    {}

    //! destractor
    ~snmpbridge(){}

    //! initilalize function
    //! @return
    int                initialize()
    {
        initialize_called = true;
        if(initialize_fail)    return -1;
        return 0;
    }

    //! finalize function
    void            finalize()
    { finalize_called = true; }

};    //class

}    //namespace    l7vs
#endif    //__SNMPBRIDGE_H__
