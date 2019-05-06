from django.contrib import admin
from .models import Warning, Loc

class LocAdmin(admin.ModelAdmin):
    list_display = ['time','longitude','latitude','magnitude', 'status']
    list_filter = ['time', 'status', ]
    search_fields = ['status']
  
class WarningAdmin(admin.ModelAdmin):
    list_display = ['time', 'color','commentary','station']
    list_filter = ['time', 'color','commentary','station']
    ordering = ['time']
    search_fields = ['color']
  

admin.site.register(Warning,WarningAdmin)
admin.site.register(Loc, LocAdmin)

