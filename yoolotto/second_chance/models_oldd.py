import datetime
import bisect
import random
import os
from django.db import models
from django.db.models.signals import post_save
from django.dispatch import receiver
from django.conf import settings
from yoolotto.util.time import format_datetime
from django.conf import settings
from django.core.files import File
import urllib


class Advertisor(models.Model):
    id = models.AutoField(primary_key=True)
    name = models.CharField(max_length=255)
    image = models.ImageField(blank=True,upload_to="static/second_chance/advertisor/", null=True,max_length=500)
    
    def representation(self):

        result = {
            "id": self.pk,
            "name": self.name,
            "image": self.image.name,
        }
               
        return result

    class Meta:
        db_table = u"advertisor"        
    
class AdInventory(models.Model):
    id = models.AutoField(primary_key=True)
    name = models.CharField(max_length=255)
    inventory = models.IntegerField(default=0)
    type = models.CharField(max_length=255, choices=(
         ("digital", "digital"),
         ("physical", "physical")
     ),default="physical")
    timer = models.BooleanField()
    time = models.IntegerField(default=0)
    account = models.ForeignKey(Advertisor, related_name="ad_inventory", blank=True, null=True)
    status = models.CharField(max_length=255)
    ad_id = models.CharField(max_length=255)
    coins = models.IntegerField(default=0)
        
    def representation(self):
        result = {
            "id": self.pk,
            "domain": "http://ox-d.yoolotto.com",
        }
        return result

    class Meta:
        db_table = u"ad_inventory"

    def __unicode__(self):
        return self.name
    
class AdIssue(models.Model):
    ad = models.ForeignKey(AdInventory, related_name="ad_inventory", blank=True, null=True)
    address = models.TextField(blank=True, null=True)
    email = models.CharField(max_length=255, blank=True, null=True)
    device = models.CharField(max_length=255, blank=True, null=True)
    phone = models.CharField(max_length=20,null=True, blank=True)
    won = models.BooleanField()
    added_at = models.DateTimeField(auto_now_add=True)
       
    def representation(self):
        result = {
            "type": "local",
            "id": self.pk,
            "parent": self.ad.representation(),
            'address': self.address,
            'email': self.email,
            'phone': self.phone,
            "added": format_datetime(self.added_at),
        }
        
        return result
    
    class Meta:
        db_table = u"ad_issue"
        
class FAQ(models.Model):
    question = models.TextField()
    answer = models.TextField()
       
    def representation(self):
        result = {
            "id": self.pk,
            "question": self.question,
            "answer": self.answer,
        }
        
        return result
    
    class Meta:
        db_table = u"faq"

        
class FiberCoins(models.Model):
    device = models.CharField(max_length=128)
    email = models.CharField(max_length=128)
    rewarded_video = models.FloatField(default=0)
    offer_wall = models.FloatField(default=0)
    class Meta:
        db_table = u"fiber_coins"


class ResetCoins(models.Model):
    user = models.ForeignKey("user.YooLottoUser", related_name="reset_coins_user")
    coins = models.FloatField(default=0)
    reset_datetime = models.DateTimeField(auto_now_add=True)
    added_at = models.DateTimeField(auto_now_add=True)
    class Meta:
        db_table = u"reset_coins"