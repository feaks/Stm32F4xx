:: 关闭回显
@echo off

:: 指定打开的文件名
set file=../../User/buildnum.h

:: 预定义文本
set text=#define BUILDNUMBER

:: 逐行遍历文件，这里只读取第一行，并将内容赋值给s
for /f "delims=" %%i in (%file%) do (set s=%%i)&(goto :next)
:next
:: 取s变量中20位后面的值自增
set /a num=%s:~20% + 1 
echo 当前版本号 %num%

:: 判断值的正确性
if %num% lss 1 set num=1

:: 拼接字符串并写入文件，末尾一定多输入一个空格，否则1位数时会出错
echo %text% %num% >%file%


:: buildnum.h    #define BUILDNUMBER 6223 

::  由用户输入
::  set /p var=请输入变量的值

::  计算    
::  set /a num =%s:~-5% + 1 

::   %s:~-5% 取s变量中的后5位
::   %s:~20% 取s变量中20位后面的值