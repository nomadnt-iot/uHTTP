uHTTP BASE
=====
<p align="right"><a href='https://pledgie.com/campaigns/28538'>
    <img alt='Click here to lend your support to: uHTTP and make a donation!' src='https://pledgie.com/campaigns/28538.png?skin_name=chrome' border='0' >
</a></p>

uHTTP Base Sample is a simple sketch that show the power of uHTTP library. The philosophy is to leave the control
to the developer. Arduino has only a little bit of memory so it is daft to build a library with many many helper and generic methods when surely you will fight with arduino memory limit.

The sketch provide provide 6 pages:

* INDEX: the index/home page;
* ADMIN: the admin area page;
* LOGIN: the login page;
* LOGOUT: the logout page;
* ERROR404: the 404 Not Found page;
* ERROR500: the 500 Internal Server Error page;

The credentials to access to admin area are:

* username: admin
* password: admin

the Base64 encoded string of username and password will be "YWRtaW46YWRtaW4="
The sketch simply check if username and password Base64 token is equals to "YWRtaW46YWRtaW4=".

This sketch is written by Filippo Sallemi <fsallemi@nomadnt.com>
