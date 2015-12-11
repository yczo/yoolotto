

from django.db import models
class adVisit(models.Model):
    device_id = models.CharField(unique=True, max_length=128)
    type  = models.CharField(max_length=255,default='text_home')
    count = models.IntegerField(default=0)
    last_visit=models.DateTimeField(auto_now=True)

    class Meta:
        db_table = u"openx_advisit"
    def increamentVisit(self):
        self.count+=1
        self.save()
#     def __unicode__(self):
#         return self.remote_id
#     
    