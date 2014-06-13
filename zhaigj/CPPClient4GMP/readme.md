##C++ Client  for Google Analytics Measurement protocol

Google recently expand its GA functionality, and are now able to process POST and GET request. This project will enable you to easily use measurement protocol in your desktop executables.

###Usage:

1. Include CPPClient4GMP project in your solution.
2. Create your GAClient by :

    `GAClient* s = GAClient::GetGAClient(%client_id%, %tracking_id_which_google_give%);`
    
3. Send page view requests by:
    
    `s->PageView(%path%, %host% = null, %title% = null);`

4. Send Customized events by:
    
    `s->Event(%module_name%, %action_name%, %label_name% = null, %value% = null);`

5. Send Exception report by: 

    `s->Exception(%exception_desc%, %fatal% = true);`
    
Usage is pretty straight forward. Code will evolve over time, Hope you like it.

###ChangeLog:

####2013-05-06 

    Enable functionality, refactor code and add "Exception" function.

####2013-03-27

    Check in init code,  create "Event" and "PageView" function.

Reach me at zhichangterry@gmail.com