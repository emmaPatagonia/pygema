"""PyGema_Web URL Configuration

The `urlpatterns` list routes URLs to views. For more information please see:
    https://docs.djangoproject.com/en/2.1/topics/http/urls/
Examples:
Function views
    1. Add an import:  from my_app import views
    2. Add a URL to urlpatterns:  path('', views.home, name='home')
Class-based views
    1. Add an import:  from other_app.views import Home
    2. Add a URL to urlpatterns:  path('', Home.as_view(), name='home')
Including another URLconf
    1. Import the include() function: from django.urls import include, path
    2. Add a URL to urlpatterns:  path('blog/', include('blog.urls'))
"""
from django.contrib import admin, auth
from django.urls import path
from PyGema_Web.views import  current_datetime ,maxtrigg ,seismicity ,maxsismicity , tolhuaca, volcanocamcall, volcanocamlonq, volcanomanz, volcanocam, volcanocall, volcanocamtol, LocList,seismicity, volcanocopa, localization , WarningList,callaqui, localization, home, warning,lonquimay, station, cop2, copa, call, maya, lonq, manz, tol1, volcano, copahue



urlpatterns = [
    path('admin/', admin.site.urls),
    path('', home),
    path('home/', home,name='home'),
    path('home/seismicity', seismicity,name='seismicity'),   
    path('time/', current_datetime),
    path('localization/', localization,name='localization'),
    path('localization/figure', localization,name='localization-figure'),
	path('home/table', LocList.as_view(),name='localization-table'),
    path('warning/', warning, name='warning'),
    path('warning/figure', warning, name='warning-figure'),
	path('warning/table', WarningList.as_view(), name='warning-table'),
	path('station/', station),
    path('volcano/', volcano, name='volcano'),
	path('station/cop2/', cop2, name='cop2'),
    path('station/copa/', copa, name='copa'),
    path('station/call/', call, name='call'),
    path('station/maya/', maya, name='maya'),
    path('station/lonq/', lonq, name='lonq'),
    path('station/manz/', manz, name='manz'),
    path('station/tol1/', tol1, name='tol1'),  
    path('volcano/copahue/', copahue, name='copahue'),   
    path('volcano/lonquimay/', lonquimay, name='lonquimay'),
    path('volcano/tolhuaca/', tolhuaca, name='tolhuaca'), 
    path('volcano/callaqui/', callaqui, name='callaqui'),
    path('volcano/copa', volcanocopa, name='volcano-copa'),
    path('volcano/cam', volcanocam, name='volcano-cam'),
    path('volcano/camcall', volcanocamcall, name='volcano-cam-call'),
    path('volcano/camlonq', volcanocamlonq, name='volcano-cam-lonq'),
    path('volcano/camtol', volcanocamtol, name='volcano-cam-tol'),
    path('volcano/call', volcanocall, name='volcano-call'),
    path('volcano/manz', volcanomanz, name='volcano-manz'),
    path('home/maxtrigg', maxtrigg, name='max-trigg'),
    path('home/maxsismicity', maxsismicity, name='max-sismicity'),
    path('home/seismicity', seismicity, name='seismicity'),

 

  
]
