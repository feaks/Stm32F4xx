:: �رջ���
@echo off

:: ָ���򿪵��ļ���
set file=../../User/buildnum.h

:: Ԥ�����ı�
set text=#define BUILDNUMBER

:: ���б����ļ�������ֻ��ȡ��һ�У��������ݸ�ֵ��s
for /f "delims=" %%i in (%file%) do (set s=%%i)&(goto :next)
:next
:: ȡs������20λ�����ֵ����
set /a num=%s:~20% + 1 
echo ��ǰ�汾�� %num%

:: �ж�ֵ����ȷ��
if %num% lss 1 set num=1

:: ƴ���ַ�����д���ļ���ĩβһ��������һ���ո񣬷���1λ��ʱ�����
echo %text% %num% >%file%


:: buildnum.h    #define BUILDNUMBER 6223 

::  ���û�����
::  set /p var=�����������ֵ

::  ����    
::  set /a num =%s:~-5% + 1 

::   %s:~-5% ȡs�����еĺ�5λ
::   %s:~20% ȡs������20λ�����ֵ