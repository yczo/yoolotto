import uuid
from django.db import models
from yoolotto.coin.models import CoinWallet

class YooLottoUser(models.Model):
    name = models.CharField(max_length=255, blank=True, null=True)
    email = models.CharField(max_length=255, blank=True, null=True, unique=True)
    #email_source = models.CharField(max_length=255,null=True, blank=True)
    password = models.CharField(max_length=255, null=True, blank=True)
    referral = models.CharField(max_length=64, null=True, blank=True)
    identifier = models.CharField(max_length=32, null=True, blank=True, unique=True)
    added_at = models.DateTimeField(auto_now_add=True)
    email_verified = models.BooleanField(default=False)

    
    def get_wallet(self):
        try:
            return self.wallet
        except CoinWallet.DoesNotExist:
            self.wallet = CoinWallet(user=self)
            self.wallet.save()
            
        return self.wallet
    
    def get_identifier(self):
        if not self.identifier:
            self.identifier = uuid.uuid4().hex
            self.save()
            
        return self.identifier
    
    def __unicode__(self):
        return "%s:%s" % (self.email, self.pk)
    
    class Meta:
        db_table = u"user"
        
class UserDetails(models.Model):
        user = models.ForeignKey(YooLottoUser, related_name="details")
        phone = models.CharField(max_length=20,null=True, blank=True)
        address = models.CharField(max_length=255,null=True, blank=True)
        
        class Meta:
            db_table = u"user_details"
            
class UserClientLogin(models.Model):
        device = models.CharField(max_length=50)
        client_login = models.CharField(max_length=255,null=True, blank=True)
        email_source = models.CharField(max_length=255,null=True, blank=True)
        
        class Meta:
            db_table = u"user_client_login"
            
class UserCoinsDetails(models.Model):
    email = models.CharField(max_length=255,null=True, blank=True)
    total_coins = models.IntegerField(default=0)
    user_coins = models.IntegerField(default=0)
    
    class Meta:
            db_table = u"user_coins_details"
            
class PasswordReset(models.Model):
    email = models.CharField(max_length=255,null=True, blank=True)
    code = models.IntegerField(max_length= 10)
    reset = models.BooleanField()
    
    class Meta:
            db_table = u"password_reset"
        
class UserToken(models.Model):
    user = models.ForeignKey(YooLottoUser, related_name="tokens")
    name = models.CharField(max_length=32)
    token = models.TextField()
    
    class Meta:
        db_table = u"user_tokens"
    
class UserPreference(models.Model):
    user = models.OneToOneField(YooLottoUser, related_name="preferences")
    
    jackpot_drawing = models.BooleanField(default=True)
    jackpot_frenzy = models.BooleanField(default=True)
    
    newsletter = models.BooleanField(default=False)
    
    class Meta:
        db_table = u"user_preference"

class Device(models.Model):
    user = models.ForeignKey(YooLottoUser, related_name="devices")
    
    device_type = models.CharField(max_length=64, default="UNKNOWN")
    device_id = models.CharField(max_length=128)
    
    # Push Notification Token
    device_token = models.CharField(max_length=192, null=True, blank=True, db_index=True)
    
    app_version = models.CharField(max_length=32, null=True, blank=True)
    os_version = models.CharField(max_length=32, null=True, blank=True)
    
    added_at = models.DateTimeField(auto_now_add=True)
    
    def is_ios(self):
        return True if self.device_type in ["IPHONE", "IPAD", "IPOD"] else False
    
    def is_android(self):
        return True if self.device_type in ["ANDROID"] else False
    
    def __unicode__(self):
        return "%s:%s" % (self.pk, self.device_id)
    
    class Meta:
        unique_together = ('user', 'device_id')
        db_table = u"user_device"

class FbDetails(models.Model):
    fb_id = models.CharField(max_length=32)
    fb_email = models.CharField(max_length=32)

    class Meta:
        db_table = u"fb_details"
