import datetime
import pytz

from django.db import models
from django.db.models.signals import post_save
from django.dispatch import receiver
from django.utils import timezone

COINS_PER_PLAY = 0
COINS_PER_DAY_MAX = 0      # For Submissions Only
COINS_PER_TICKET_MAX = 0

class CoinWallet(models.Model):
    user = models.OneToOneField("user.YooLottoUser", related_name="wallet")
    coins = models.IntegerField(default=0)
    
    def update(self):
        coins = self.transactions.all().aggregate(models.Sum('amount'))["amount__sum"]
        save = False
        
        if coins != self.coins:
            save = True
            self.coins = coins
            
        if save:
            self.save()
            
        return save
    
    class Meta:
        db_table = u"coin_wallet"
        

class CoinTransaction(models.Model):
    wallet = models.ForeignKey(CoinWallet, related_name="transactions")
    amount = models.IntegerField(default=0)
    added_at = models.DateTimeField(null=True, auto_now_add=True)
    
    def representation(self):
        return self.amount
    
    class Meta:
        db_table = u"coin_transaction"
        

class DeviceCoins(models.Model):
    device_id = models.CharField(max_length=128)
    coins = models.FloatField(default=0)
    reset_date = models.DateTimeField(auto_now_add=True)
    invites = models.IntegerField(default=0)
    messages = models.IntegerField(default=0)
    coins_transfered = models.BooleanField(default=False)

    class Meta:
        db_table = u'device_coins'

    def get_coins(self):
        return self.coins

class EmailCoins(models.Model):
    email = models.CharField(max_length=128)
    coins = models.FloatField(default=0)
    reset_date = models.DateTimeField(auto_now_add=True)
    invites = models.IntegerField(default=0)
    messages = models.IntegerField(default=0)
    share = models.IntegerField(default=0)
    likes = models.IntegerField(default=0)
    dollar_amount = models.FloatField(default=0)
    
    class Meta:
        db_table = u'email_coins'

    def get_coins(self):
        return self.coins
        


class CoinSubmissionTransaction(models.Model):
#    objects = CoinTicketTransactionManager()
    
    transaction = models.ForeignKey(CoinTransaction, related_name="transaction_submission")
    submission = models.ForeignKey("lottery.LotteryTicketSubmission", unique=True, 
        related_name="transaction_submission")
    
    # Indicates Number of Plays for Submission
#    count = models.IntegerField(default=0)
    
    @property
    def amount(self):
        raise RuntimeError()
    
    class Meta:
        db_table = u"coin_transaction_submission"
        
class CoinTicketTransaction(models.Model):
#    objects = CoinTicketTransactionManager()
    
    transaction = models.ForeignKey(CoinTransaction, related_name="transaction_ticket")
    ticket = models.ForeignKey("lottery.LotteryTicket", unique=True, related_name="coin_ticket_transaction")
    
    # Indicates whether this ticket was checked (at least once)
    checked = models.NullBooleanField(default=False)
    
    @property
    def amount(self):
        raise RuntimeError()
    
    class Meta:
        db_table = u"coin_transaction_ticket"
        
class CoinShareTransaction(models.Model):
    transaction = models.ForeignKey(CoinTransaction, related_name="transaction_share")
    ticket = models.ForeignKey("lottery.LotteryTicket", unique=True, related_name="coin_share_transaction")
    type = models.CharField(max_length=64)
    
    @property
    def amount(self):
        return 0
    
    class Meta:
        db_table = u"coin_transaction_share"
        
class CoinTicketGenericTransaction(models.Model):
    transaction = models.ForeignKey(CoinTransaction, related_name="transaction_generic")
    user = models.ForeignKey("user.YooLottoUser", related_name="coin_generic_transaction")
    type = models.CharField(max_length=64)
    1
    @property
    def amount(self):
        return 0
            
    class Meta:
        db_table = u"coin_transaction_generic"
    
@receiver(post_save, sender=CoinTransaction)
def update_wallet_from_transaction(sender, instance, signal, created, **kwargs):
    
    # Due to the model-creation process, zero-amount transactions don't yet have 
    # an actual event attached, there is no reason to trigger a calculation
    if created and instance.amount == 0:
        return
    
    instance.wallet.update()
