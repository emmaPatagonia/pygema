from django.template.loader import get_template
from django.template import Template, Context
from django.http import HttpResponse
from django.shortcuts import render_to_response, render
from PyGema_Web.models import Loc, Warning as war, Rsam
from django.views.generic import ListView

import datetime


def current_datetime(request):
    now = datetime.datetime.now()
    return render_to_response('current_datetime.html', {'current_date': now})

class LocList(ListView):
	queryset = Loc.objects.all()
	paginate_by = 50
	context_object_name = 'loc'
	ordering = ['-time']

class WarningList(ListView):
    queryset = war.objects.all()
    paginate_by = 10
    context_object_name = 'war'
    ordering = ['-time']

def localization(request):
    return render(request, "localization.html")

def volcanocamlonq(request):
    return render(request, "PyGema_Web/volcano-cam-lonquimay.html")

def volcanocamcall(request):
    return render(request, "PyGema_Web/volcano-cam-callaqui.html")

def volcanocam(request):
    return render(request, "PyGema_Web/volcano-cam.html")    

def volcanocall(request):
    return render(request, "PyGema_Web/volcano-call.html")

def volcanomanz(request):
    return render(request, "PyGema_Web/volcano-manz.html")

def cop2(request):
	return render(request, "PyGema_Web/cop2.html")

def copa(request):
    return render(request, "PyGema_Web/copa.html")

def call(request):
    return render(request, "PyGema_Web/call.html")

def maya(request):
    return render(request, "PyGema_Web/maya.html")        

def lonq(request):
    return render(request, "PyGema_Web/lonq.html")

def tol1(request):
    return render(request, "PyGema_Web/tol1.html")     

def maya(request):
    return render(request, "PyGema_Web/maya.html") 

def manz(request):
    return render(request, "PyGema_Web/manz.html")     

def home(request):
    return render(request, "home.html")

def warning(request):
    return render(request, "warning.html")    


def station(request):
    return render(request, "station.html") 

def volcano(request):
    return render(request, "volcano.html") 

def copahue(request):
    return render(request, "PyGema_Web/copahue.html") 

def lonquimay(request):
    return render(request, "PyGema_Web/lonquimay.html") 

def callaqui(request):
    return render(request, "PyGema_Web/callaqui.html")

def tolhuaca(request):
    return render(request, "PyGema_Web/tolhuaca.html")        

def seismicity(request):
    return render(request, "PyGema_Web/seismicity.html")         

def volcanocopa(request):
    return render(request, "PyGema_Web/volcano-copa.html") 

def maxtrigg(request):
    return render(request, "Maximize/max-trigg.html") 

def seismicity(request):
    return render(request, "PyGema_Web/seismicity.html")

def  maxsismicity(request):
    return render(request, "Maximize/max-sismicity.html")
    pass

def volcanocamtol(request):
    return render(request,"PyGema_Web/volcano-cam-tolhuaca.html")






