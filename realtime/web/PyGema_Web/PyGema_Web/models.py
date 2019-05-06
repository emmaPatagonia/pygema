# This is an auto-generated Django model module.
# You'll have to do the following manually to clean this up:
#   * Rearrange models' order
#   * Make sure each model has one field with primary_key=True
#   * Make sure each ForeignKey has `on_delete` set to the desired behavior.
#   * Remove `managed = False` lines if you wish to allow Django to create, modify, and delete the table
# Feel free to rename the models, but don't rename db_table values or field names.
from django.db import models


class Autoloc(models.Model):
    time = models.DateTimeField(primary_key=True)
    longitude = models.FloatField(blank=True, null=True)
    latitude = models.FloatField(blank=True, null=True)
    depth = models.FloatField(blank=True, null=True)
    number_of_stations = models.FloatField(blank=True, null=True)
    gap = models.FloatField(blank=True, null=True)
    rms = models.FloatField(blank=True, null=True)
    status = models.CharField(max_length=15, blank=True, null=True)
    magnitude = models.FloatField()

    class Meta:
        managed = False
        db_table = 'AUTOLOC'


class Autotriggers(models.Model):
    station = models.CharField(primary_key=True, max_length=4)
    trig_on = models.DateTimeField()
    trig_off = models.DateTimeField()

    class Meta:
        managed = False
        db_table = 'AUTOTRIGGERS'
        unique_together = (('station', 'trig_on'),)


class Frequency(models.Model):
    frequency = models.IntegerField(primary_key=True)

    class Meta:
        managed = False
        db_table = 'FREQUENCY'


class Frequency32(models.Model):
    frequency32 = models.FloatField()

    class Meta:
        managed = False
        db_table = 'FREQUENCY_32'


class FrequencyData(models.Model):
    frequency = models.ForeignKey(Frequency, models.DO_NOTHING, db_column='frequency')
    data = models.FloatField()

    class Meta:
        managed = False
        db_table = 'FREQUENCY_DATA'


class Loc(models.Model):
    time = models.DateTimeField(primary_key=True)
    longitude = models.FloatField(blank=True, null=True)
    latitude = models.FloatField(blank=True, null=True)
    depth = models.FloatField(blank=True, null=True)
    number_of_stations = models.FloatField(blank=True, null=True)
    gap = models.FloatField()
    rms = models.FloatField(blank=True, null=True)
    dx = models.FloatField(blank=True, null=True)
    dy = models.FloatField(blank=True, null=True)
    dz = models.FloatField(blank=True, null=True)
    magnitude = models.FloatField(blank=True, null=True)
    status = models.CharField(max_length=15, blank=True, null=True)

    class Meta:
        managed = False
        db_table = 'LOC'

    def __str__(self):
        return '%s %s %s %s %s' % (self.time, self.longitude, self.latitude, self.magnitude, self.status)    



class Warning(models.Model):
    time = models.DateTimeField(primary_key=True)
    color = models.CharField(max_length=10)
    commentary = models.CharField(max_length=200)
    station = models.CharField(max_length=4)

    class Meta:
        managed = False
        db_table = 'WARNING'
        ordering = ["time"]

    def __str__(self):
        return '%s %s %s' % (self.time, self.color, self.station)



class Rsam(models.Model):
    rsam = models.FloatField()
    station = models.CharField(max_length=8)
    time = models.DateTimeField()

    class Meta:
        managed = False
        db_table = 'RSAM'
    def __str__(self):
        return '%s %s %s' % (self.rsam, self.station, self.time)    


class Ssam(models.Model):
    ssam_time = models.DateTimeField(primary_key=True)
    frequency = models.IntegerField()
    station = models.CharField(max_length=10)

    class Meta:
        managed = False
        db_table = 'SSAM'


class SsamData(models.Model):
    amplitude = models.FloatField()
    time = models.DateTimeField()
    frequency = models.FloatField(blank=True, null=True)

    class Meta:
        managed = False
        db_table = 'SSAM_DATA'



class AppOrganization(models.Model):
    name = models.CharField(max_length=100)

    class Meta:
        managed = False
        db_table = 'app_organization'


class AppPerson(models.Model):
    name = models.CharField(max_length=100)
    organization = models.ForeignKey(AppOrganization, models.DO_NOTHING, blank=True, null=True)
    married = models.IntegerField()

    class Meta:
        managed = False
        db_table = 'app_person'


class AuthGroup(models.Model):
    name = models.CharField(unique=True, max_length=80)

    class Meta:
        managed = False
        db_table = 'auth_group'


class AuthGroupPermissions(models.Model):
    group = models.ForeignKey(AuthGroup, models.DO_NOTHING)
    permission = models.ForeignKey('AuthPermission', models.DO_NOTHING)

    class Meta:
        managed = False
        db_table = 'auth_group_permissions'
        unique_together = (('group', 'permission'),)


class AuthPermission(models.Model):
    name = models.CharField(max_length=255)
    content_type = models.ForeignKey('DjangoContentType', models.DO_NOTHING)
    codename = models.CharField(max_length=100)

    class Meta:
        managed = False
        db_table = 'auth_permission'
        unique_together = (('content_type', 'codename'),)


class AuthUser(models.Model):
    password = models.CharField(max_length=128)
    last_login = models.DateTimeField(blank=True, null=True)
    is_superuser = models.IntegerField()
    username = models.CharField(unique=True, max_length=150)
    first_name = models.CharField(max_length=30)
    last_name = models.CharField(max_length=150)
    email = models.CharField(max_length=254)
    is_staff = models.IntegerField()
    is_active = models.IntegerField()
    date_joined = models.DateTimeField()

    class Meta:
        managed = False
        db_table = 'auth_user'


class AuthUserGroups(models.Model):
    user = models.ForeignKey(AuthUser, models.DO_NOTHING)
    group = models.ForeignKey(AuthGroup, models.DO_NOTHING)

    class Meta:
        managed = False
        db_table = 'auth_user_groups'
        unique_together = (('user', 'group'),)


class AuthUserUserPermissions(models.Model):
    user = models.ForeignKey(AuthUser, models.DO_NOTHING)
    permission = models.ForeignKey(AuthPermission, models.DO_NOTHING)

    class Meta:
        managed = False
        db_table = 'auth_user_user_permissions'
        unique_together = (('user', 'permission'),)


class DjangoAdminLog(models.Model):
    action_time = models.DateTimeField()
    object_id = models.TextField(blank=True, null=True)
    object_repr = models.CharField(max_length=200)
    action_flag = models.PositiveSmallIntegerField()
    change_message = models.TextField()
    content_type = models.ForeignKey('DjangoContentType', models.DO_NOTHING, blank=True, null=True)
    user = models.ForeignKey(AuthUser, models.DO_NOTHING)

    class Meta:
        managed = False
        db_table = 'django_admin_log'


class DjangoContentType(models.Model):
    app_label = models.CharField(max_length=100)
    model = models.CharField(max_length=100)

    class Meta:
        managed = False
        db_table = 'django_content_type'
        unique_together = (('app_label', 'model'),)


class DjangoMigrations(models.Model):
    app = models.CharField(max_length=255)
    name = models.CharField(max_length=255)
    applied = models.DateTimeField()

    class Meta:
        managed = False
        db_table = 'django_migrations'


class DjangoSession(models.Model):
    session_key = models.CharField(primary_key=True, max_length=40)
    session_data = models.TextField()
    expire_date = models.DateTimeField()

    class Meta:
        managed = False
        db_table = 'django_session'


class DjangoSite(models.Model):
    domain = models.CharField(unique=True, max_length=100)
    name = models.CharField(max_length=50)

    class Meta:
        managed = False
        db_table = 'django_site'
