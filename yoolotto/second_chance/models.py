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
    email = models.CharField(max_length=255)
    rewarded_video = models.FloatField(default=0)
    offer_wall = models.FloatField(default=0)
    class Meta:
        db_table = u"fiber_coins"

class DeviceLoginDetails(models.Model):
    user = models.ForeignKey("user.YooLottoUser", related_name="reset_coins_user")
    device = models.CharField(max_length=255)
    email = models.CharField(max_length=255)

    class Meta:
        db_table = u"device_login_details"

class DwollaUserInfo(models.Model):
    user = models.ForeignKey("user.YooLottoUser", related_name="dwolla_user")
    email = models.CharField(max_length=255)
    dwolla_email = models.CharField(max_length=255)
    name = models.CharField(max_length=255)
    dwolla_amount = models.FloatField(default=0)
    phone = models.BigIntegerField(default=0)
    dwolla_pin = models.IntegerField(default=0)
    total_amount = models.FloatField(default=0)
    isDwollaUser = models.BooleanField(default=1)
    added_at = models.DateTimeField(auto_now_add=True)
    class Meta:
        db_table = u"dwolla_user_info"

class DwollaTransactionInfo(models.Model):
    dwolla_detail = models.ForeignKey("second_chance.DwollaUserInfo")
    source = models.CharField(max_length=255)
    source_info = models.CharField(max_length=255)
    amount_requested = models.FloatField(default=0)
    added_at = models.DateTimeField(auto_now_add=True)
    transaction_id = models.CharField(max_length=255)

    class Meta:
        db_table = u"dwolla_transaction_info"

class FantasyFAQ(models.Model):
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
        db_table = u"fantasy_faq"


class Events(models.Model):
    data = models.TextField()
    class Meta:
	db_table = u"events"

class EventInfo(models.Model):
    referring_user = models.CharField(max_length=255)
    referred_user = models.CharField(max_length=255)
    event_occured = models.CharField(max_length=255)
    referring_user_coins = models.FloatField(default=0)
    referred_user_coins = models.FloatField(default=0)
    
    class Meta:
        db_table = u"events_info"


class DwollaFAQ(models.Model):
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
        db_table = u"dwolla_faq"
