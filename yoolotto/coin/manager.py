import datetime
import pytz

from django.db import models
from django.utils import timezone

from yoolotto.coin.models import COINS_PER_PLAY, COINS_PER_DAY_MAX, \
    CoinTransaction, CoinSubmissionTransaction, CoinTicketTransaction, EmailCoins, DeviceCoins


from django.db.models import F
    
class CoinTicketManager(object):
    """
    Responsible for determining and updating the number of YooCoins granted to 
    a user for a ticket. Enforces daily limits for coins gained from submissions
    
    """
    
    def __init__(self, device, user_email, ticket):
        self._ticket = ticket
        self._wallet = ticket.user.get_wallet()
        self._allocated = None
        self._device = device
        self._user_email = user_email
        self._valid = True        
    @property
    def allocated(self):
        
        if self._allocated is None:
            raise RuntimeError("Update Coins BEFORE requesting allocated")
        
        return self._allocated if self._allocated else None

    # function to add 1 coin to email , if someone is logged in, else 1 coin will be added to device    
    def add_coins(self):
        if self._user_email:
            coins_record = EmailCoins.objects.get(email=self._user_email)
        else:
            try:
        
                coins_record, created = DeviceCoins.objects.get_or_create(device_id=self._device, defaults={'coins': 0})
        
            except:
          
                coins_record = DeviceCoins.objects.filter(device_id=self._device)[0]
            
        coins_record.coins = F('coins') + 0
        coins_record.save()
          
    
    def update(self):
        if self._device:
         self._allocated = 0
        
                    
         # Process Checked v.s. Unchecked
         if not CoinTicketTransaction.objects.filter(ticket=self._ticket).exists():
            transaction = CoinTransaction(wallet=self._wallet)
            transaction.save()

            try:
                    self.add_coins()
            except:
                pass
    
            # Create Record linking Ticket to Transaction
            transaction_ticket = CoinTicketTransaction(ticket=self._ticket,  transaction=transaction)
            transaction_ticket.save()
         else:
            transaction_ticket = CoinTicketTransaction.objects.get(ticket=self._ticket)
            transaction = transaction_ticket.transaction
            
         # Allocate Coins
         # Changed: This will never de-allocate a coin, if you submit and check 
         # ONCE you get the coin, if you add more submissions and do not check 
         # this coin is not lost.
         coins = COINS_PER_PLAY if self._ticket.unchecked == 0 else 0
         if transaction.amount < coins:
            self.add_coins()
            transaction.amount = coins
            transaction_ticket.checked = True
            transaction.save()
            transaction_ticket.save()
            self._allocated += coins
            
         del transaction
        
         # Process for Submissions
         # For every submission we grant COINS_PER_PLAY __regardless__ of how 
         # many plays there are for that submission. In addition there is a limit 
         # of COINS_PER_DAY_MAX via this mechanism (others are not effected) 
        
         submissions = self._ticket.submissions.all()
        
         for submission in submissions:
            if not CoinSubmissionTransaction.objects.filter(submission=submission).exists():
                transaction = CoinTransaction(wallet=self._wallet)
                transaction.save()
                                
                # Create Record linking Ticket to Transaction
                transaction_submission = CoinSubmissionTransaction(submission=submission,  transaction=transaction)
                transaction_submission.save()
            else:
                transaction_submission = CoinSubmissionTransaction.objects.get(submission=submission)
                transaction = transaction_submission.transaction
                
            coins = COINS_PER_PLAY
            delta = coins - transaction.amount
            
           
            if not delta > 0:
                continue
            
            # Maximum number of submission coins that remain to be granted for 
            # the day
            remaining = COINS_PER_DAY_MAX - \
                CoinTicketManager.for_user_on_day(self._wallet.user, submission.added_at)
            
            
            if not remaining:
                continue
            
                transaction.amount = coins
                transaction.save()
                
                self._allocated += coins
            else:
                self._allocated = 0
        
            # Process Checked v.s. Unchecked
            if not CoinTicketTransaction.objects.filter(ticket=self._ticket).exists():
             transaction = CoinTransaction(wallet=self._wallet)
             transaction.save()
            
             # Create Record linking Ticket to Transaction
             transaction_ticket = CoinTicketTransaction(ticket=self._ticket,  transaction=transaction)
             transaction_ticket.save()
            else:
             transaction_ticket = CoinTicketTransaction.objects.get(ticket=self._ticket)
             transaction = transaction_ticket.transaction
            
            # Allocate Coins
            # Changed: This will never de-allocate a coin, if you submit and check 
            # ONCE you get the coin, if you add more submissions and do not check 
            # this coin is not lost.
            coins = COINS_PER_PLAY if self._ticket.unchecked == 0 else 0
            if transaction.amount < coins:
             transaction.amount = coins
             transaction_ticket.checked = True
             transaction.save()
             transaction_ticket.save()
             self._allocated += coins
            
            del transaction
        
            # Process for Submissions
            # For every submission we grant COINS_PER_PLAY __regardless__ of how 
            # many plays there are for that submission. In addition there is a limit 
            # of COINS_PER_DAY_MAX via this mechanism (others are not effected) 
        
            submissions = self._ticket.submissions.all()
        
            for submission in submissions:
             if not CoinSubmissionTransaction.objects.filter(submission=submission).exists():
                transaction = CoinTransaction(wallet=self._wallet)
                transaction.save()
                
                # Create Record linking Ticket to Transaction
                transaction_submission = CoinSubmissionTransaction(submission=submission,  transaction=transaction)
                transaction_submission.save()
             else:
                transaction_submission = CoinSubmissionTransaction.objects.get(submission=submission)
                transaction = transaction_submission.transaction
                
             coins = COINS_PER_PLAY
             delta = coins - transaction.amount
            
            
             if not delta > 0:
                continue
            
             # Maximum number of submission coins that remain to be granted for 
             # the day
             remaining = COINS_PER_DAY_MAX - \
                CoinTicketManager.for_user_on_day(self._wallet.user, submission.added_at)
            

            
             if not remaining:
                continue
            
             transaction.amount = coins
             transaction.save()
            
             self._allocated += coins    

    @classmethod
    def for_user_on_day(cls, user, date=None):
        """
        Calculates the amount of YooCoins allocated for a user on a particular 
        day
        
        """
        central = pytz.timezone('US/Central')
        default = timezone.get_default_timezone()
        
        # Get Local Datetime
        if not date:
            date = datetime.datetime.now(default)
        else:
            date = timezone.make_aware(date, default)
            
        # Convert to Central, Snap to Beginning of Day, Convert back
        date = date.astimezone(central)
        min = datetime.datetime(date.year, date.month, date.day, 0, 0, 0)
        min = timezone.make_aware(min, central)
        min = min.astimezone(default)
        min = min.replace(tzinfo=None)
        
        max = min + datetime.timedelta(days=1)
        
        # Retrieve user transactions (for tickets) in the time range
        transactions = CoinSubmissionTransaction.objects.filter(
            transaction__wallet__user=user, transaction__added_at__gte=min, 
            transaction__added_at__lt=max)
        
        transactions = transactions.aggregate(models.Sum("transaction__amount"))
        
        result = transactions["transaction__amount__sum"] if \
            transactions["transaction__amount__sum"] else 0
            
        return result

        
class CoinTicketManagerNew(object):
    """
    Responsible for determining and updating the number of YooCoins granted to 
    a user for a ticket. Enforces daily limits for coins gained from submissions
    
    """
    
    def __init__(self, device,count_tick, user_email,ticket):
        self._ticket = ticket
        self._wallet = ticket.user.get_wallet()
        self._allocated = None
        self._device = device
	self._count_tick = count_tick
        self._user_email = user_email
	self._valid = True        
    @property
    def allocated(self):
        
        if self._allocated is None:
            raise RuntimeError("Update Coins BEFORE requesting allocated")
        
        return self._allocated if self._allocated else None

    # function to add 1 coin to email , if someone is logged in, else 1 coin will be added to device    
    def add_coins_on_submission(self):
        from yoolotto.lottery.models import LotteryTicketAvailable
        import datetime
        now = datetime.datetime.now()
        req_date = datetime.datetime(now.year, now.month, now.day)
        count1=0
        
        if self._user_email is not None and self._user_email != '':
            try:
                email_coins_record = EmailCoins.objects.filter(email = self._user_email,)[0]
            except:
                email_coins_record, created = EmailCoins.objects.get_or_create(email = self._user_email, defaults={'coins': 0})
        count1 = LotteryTicketAvailable.objects.filter(device = self._device, added_at__gte = req_date, ticket__fantasy = False, available = True).values("ticket_id").distinct().count()
        if count1 > self._count_tick:
            email_coins_record.coins = email_coins_record.coins - 0
            #amount =float(2)/100
            amount = 0
            email_coins_record.dollar_amount = email_coins_record.dollar_amount - amount
            email_coins_record.save()
        else:
            email_coins_record.coins = email_coins_record.coins + 0
            #amount=float(5)/100
            amount = 0
            email_coins_record.dollar_amount = email_coins_record.dollar_amount - amount
            email_coins_record.save()
        
    def add_coins(self):
        if self._user_email:
            coins_record = EmailCoins.objects.get(email=self._user_email)
        else:
	    try:
		
            	coins_record, created = DeviceCoins.objects.get_or_create(device_id=self._device, defaults={'coins': 0})
		
	    except:
	      
		coins_record = DeviceCoins.objects.filter(device_id=self._device)[0]
            
        coins_record.coins = F('coins') + 10
        coins_record.save()
	      
    def update_new_release(self):
        if self._device:
            self._allocated = 0

        if self._ticket.fantasy == False:
            try:
                self.add_coins_on_submission()
            except:
                pass

        '''if not CoinTicketTransaction.objects.filter(ticket=self._ticket).exists():
            transaction = CoinTransaction(wallet=self._wallet)
            transaction.save()

    
            # Create Record linking Ticket to Transaction
            transaction_ticket = CoinTicketTransaction(ticket=self._ticket,  transaction=transaction)
            transaction_ticket.save()
        else:
            transaction_ticket = CoinTicketTransaction.objects.get(ticket=self._ticket)
            transaction = transaction_ticket.transaction '''   

    def update(self):
	if self._device:
         self._allocated = 0
        
                    
         # Process Checked v.s. Unchecked
         if not CoinTicketTransaction.objects.filter(ticket=self._ticket).exists():
            transaction = CoinTransaction(wallet=self._wallet)
            transaction.save()

	    try:
            	self.add_coins_on_submission()
	    except:
		pass
    
            # Create Record linking Ticket to Transaction
            transaction_ticket = CoinTicketTransaction(ticket=self._ticket,  transaction=transaction)
            transaction_ticket.save()
         else:
            transaction_ticket = CoinTicketTransaction.objects.get(ticket=self._ticket)
            transaction = transaction_ticket.transaction
            
         # Allocate Coins
         # Changed: This will never de-allocate a coin, if you submit and check 
         # ONCE you get the coin, if you add more submissions and do not check 
         # this coin is not lost.
         coins = COINS_PER_PLAY if self._ticket.unchecked == 0 else 0
         if transaction.amount < coins:
            #self.add_coins()
            transaction.amount = coins
            transaction_ticket.checked = True
            transaction.save()
            transaction_ticket.save()
            self._allocated += coins
            
         del transaction
        
         # Process for Submissions
         # For every submission we grant COINS_PER_PLAY __regardless__ of how 
         # many plays there are for that submission. In addition there is a limit 
         # of COINS_PER_DAY_MAX via this mechanism (others are not effected) 
        
         submissions = self._ticket.submissions.all()
        
         for submission in submissions:
            if not CoinSubmissionTransaction.objects.filter(submission=submission).exists():
                transaction = CoinTransaction(wallet=self._wallet)
                transaction.save()
                                
                # Create Record linking Ticket to Transaction
                transaction_submission = CoinSubmissionTransaction(submission=submission,  transaction=transaction)
                transaction_submission.save()
            else:
                transaction_submission = CoinSubmissionTransaction.objects.get(submission=submission)
                transaction = transaction_submission.transaction
                
            coins = COINS_PER_PLAY
            delta = coins - transaction.amount
            
           
            if not delta > 0:
                continue
            
            # Maximum number of submission coins that remain to be granted for 
            # the day
            remaining = COINS_PER_DAY_MAX - \
                CoinTicketManager.for_user_on_day(self._wallet.user, submission.added_at)
            
            
            if not remaining:
                continue
            
            transaction.amount = coins
            transaction.save()
            
            self._allocated += coins
        else:
	    self._allocated = 0
        
            # Process Checked v.s. Unchecked
            if not CoinTicketTransaction.objects.filter(ticket=self._ticket).exists():
             transaction = CoinTransaction(wallet=self._wallet)
             transaction.save()
            
             # Create Record linking Ticket to Transaction
             transaction_ticket = CoinTicketTransaction(ticket=self._ticket,  transaction=transaction)
             transaction_ticket.save()
            else:
             transaction_ticket = CoinTicketTransaction.objects.get(ticket=self._ticket)
             transaction = transaction_ticket.transaction
            
            # Allocate Coins
            # Changed: This will never de-allocate a coin, if you submit and check 
            # ONCE you get the coin, if you add more submissions and do not check 
            # this coin is not lost.
            coins = COINS_PER_PLAY if self._ticket.unchecked == 0 else 0
            if transaction.amount < coins:
             transaction.amount = coins
             transaction_ticket.checked = True
             transaction.save()
             transaction_ticket.save()
             self._allocated += coins
            
            del transaction
        
            # Process for Submissions
            # For every submission we grant COINS_PER_PLAY __regardless__ of how 
            # many plays there are for that submission. In addition there is a limit 
            # of COINS_PER_DAY_MAX via this mechanism (others are not effected) 
        
            submissions = self._ticket.submissions.all()
        
            for submission in submissions:
             if not CoinSubmissionTransaction.objects.filter(submission=submission).exists():
                transaction = CoinTransaction(wallet=self._wallet)
                transaction.save()
                
                # Create Record linking Ticket to Transaction
                transaction_submission = CoinSubmissionTransaction(submission=submission,  transaction=transaction)
                transaction_submission.save()
             else:
                transaction_submission = CoinSubmissionTransaction.objects.get(submission=submission)
                transaction = transaction_submission.transaction
                
             coins = COINS_PER_PLAY
             delta = coins - transaction.amount
            
            
             if not delta > 0:
                continue
            
             # Maximum number of submission coins that remain to be granted for 
             # the day
             remaining = COINS_PER_DAY_MAX - \
                CoinTicketManager.for_user_on_day(self._wallet.user, submission.added_at)
            

            
             if not remaining:
                continue
            
             transaction.amount = coins
             transaction.save()
            
             self._allocated += coins    

    @classmethod
    def for_user_on_day(cls, user, date=None):
        """
        Calculates the amount of YooCoins allocated for a user on a particular 
        day
        
        """
        central = pytz.timezone('US/Central')
        default = timezone.get_default_timezone()
        
        # Get Local Datetime
        if not date:
            date = datetime.datetime.now(default)
        else:
            date = timezone.make_aware(date, default)
            
        # Convert to Central, Snap to Beginning of Day, Convert back
        date = date.astimezone(central)
        min = datetime.datetime(date.year, date.month, date.day, 0, 0, 0)
        min = timezone.make_aware(min, central)
        min = min.astimezone(default)
        min = min.replace(tzinfo=None)
        
        max = min + datetime.timedelta(days=1)
        
        # Retrieve user transactions (for tickets) in the time range
        transactions = CoinSubmissionTransaction.objects.filter(
            transaction__wallet__user=user, transaction__added_at__gte=min, 
            transaction__added_at__lt=max)
        
        transactions = transactions.aggregate(models.Sum("transaction__amount"))
        
        result = transactions["transaction__amount__sum"] if \
            transactions["transaction__amount__sum"] else 0
            
        return result
