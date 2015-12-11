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

COUPON_WEIGHTED = True
COUPON_REDEEM_LIFESPAN = 900

class CouponVendor(models.Model):
    name = models.CharField(max_length=255)
    #image = models.CharField(max_length=255, blank=True, null=True)
    image = models.ImageField(blank=True,upload_to="static/coupon/vendor/", null=True)
    wallet_url = models.CharField(max_length=1024, blank=True, null=True)
    coupon_url = models.CharField(max_length=1024, blank=True, null=True)
    coupon_mini_url = models.CharField(max_length=1024, blank=True, null=True)
    
    referral = models.CharField(max_length=128, blank=True, null=True)
    email_content=models.FileField(upload_to="static/coupon/vendor_email_content",null=True,blank=True)
    def representation(self, promo=None, user=None):
        now = datetime.datetime.now()
        unused = None
        if user and not self.wallet_url:
            #unused = CouponIssue.objects.filter(coupon__vendor=self, user=user, 
            #    redeemed__isnull=True, coupon__redeemable=True, coupon__valid_to__gt=now).count()
            #unused = CouponIssue.objects.filter(coupon__vendor=self).count()
            unused = Coupon.objects.filter(vendor=self, valid_to__gte=now,redeem_limit__gte=1).count()

        result = {
            "id": self.pk,
            "name": self.name,
            "image": self.image.name,
            "unused": unused,
            "wallet_url": None,
            "coupon_url": None,
            "coupon_mini_url": None
        }

        if user and self.wallet_url:
            identifier = user.get_identifier()
            result["wallet_url"] = self.wallet_url# % identifier
            
            if promo:
                result["coupon_url"] = self.coupon_url % (identifier, promo.promo)
                result["coupon_mini_url"] = self.coupon_mini_url % (identifier, promo.promo)       
        return result


    class Meta:
        db_table = u"coupon_vendor"

    def __unicode__(self):
        return self.name
        
class CouponVendorPromo(models.Model):
    promo = models.CharField(max_length=255)    
    vendor = models.ForeignKey(CouponVendor, related_name="promos")
    
    # Limitations
    valid_from = models.DateTimeField(null=True, blank=True)
    valid_to = models.DateTimeField(null=True, blank=True)
        
    added_at = models.DateTimeField(auto_now_add=True)
    
    @classmethod
    def choose(cls):
        """
        Return an available coupon if one is available. 
        weighted - bias against low-availability coupons
        
        """
        now = datetime.datetime.now()
        
        # Filter for redeemable promos valid after now and valid for at least 
        # one day
        available = cls.objects.filter(valid_from__lte=now, 
           valid_to__gte=now + datetime.timedelta(days=1))
        
        if not available:
            return None
        
        available = list(available)
        return random.choice(available)
    
    def representation(self, user=None):
        return {
            "type": "remote",
            "expires": format_datetime(self.valid_to),
            "vendor": self.vendor.representation(promo=self, user=user)
        }
    
    class Meta:
        db_table = u"coupon_vendor_promo"
    
class Coupon(models.Model):
    title = models.CharField(max_length=255)
    description = models.CharField(max_length=255, blank=True, null=True)
    vendor = models.ForeignKey(CouponVendor, blank=True, null=True, related_name="coupons")
    url = models.URLField(blank=True,null=True)
    image = models.ImageField(blank=True,upload_to="static/coupon/vendor", null=True)
    # Geography
    geography = models.ForeignKey("coupon.CouponGeography", blank=True, null=True)
    
    # Type
    allocate_tickets = models.BooleanField(default=False)
    allocate_wallet = models.BooleanField(default=False)
    
    # Limitations
    valid_from = models.DateTimeField(null=True, blank=True)
    valid_to = models.DateTimeField(null=True, blank=True)
    redeem_limit = models.IntegerField(blank=True, null=True)
    redeem_strict = models.BooleanField(default=False)
    
    # Aggregates
    # Determines whether this coupon is redeemable based upon the redeemable 
    # limit, does _not_ change based on chronological validity
    redeemable = models.BooleanField(default=True)
    
    # Counts
    issued_total = models.IntegerField(default=0)
    issued_redeemed = models.IntegerField(default=0)
    sequence = models.IntegerField(default=0)
    vote_liked = models.IntegerField(default=0)
    vote_disliked = models.IntegerField(default=0)
    
    added_at = models.DateTimeField(auto_now_add=True)
    
    def update_issue(self, created):
        """
        Invoked via Signal
        
        Updates the aggregate issued, redeemed, and redeemable.
        returns boolean indicating if saved
        
        """
        issued = self.issued.count()        
        redeemed = self.issued.filter(redeemed__isnull=False).count()
        redeemable = True
        
        if self.redeem_limit and redeemed >= self.redeem_limit:
            redeemable = False
        
        save = False
        if self.issued_total != issued:
            save = True
            self.issued_total = issued
            
        if self.issued_redeemed != redeemed:
            save = True
            self.issued_redeemed = redeemed
            
        if self.redeemable != redeemable:
            save = True
            self.redeemable = redeemable
            
        if save:
            self.save()
            
        return save
        
    def update_votes(self, created=False):
        """
        Invoked via Signal
        
        Updates the aggregate likes and dis-likes for a the Coupon
        returns boolean indicating if saved
        
        """
        liked = self.votes.filter(liked=True).count()
        disliked = self.votes.filter(liked=False).count()
        
        save = False
        if self.vote_liked != liked:
            save = True
            self.vote_liked = liked
            
        if self.vote_disliked != disliked:
            save = True
            self.vote_disliked = disliked
            
        if save:
            self.save()
            
        return save
    
    @classmethod
    def choose(cls, ticket=None, user=None, weighted=COUPON_WEIGHTED):
        """
        Return an available coupon if one is available. 
        weighted - bias against low-availability coupons
        
        """
        now = datetime.datetime.now()
        
        # Filter for redeemable coupons valid after now and valid for at least 
        # one day
        available = cls.objects.filter(valid_from__lte=now, 
           valid_to__gte=now + datetime.timedelta(days=1), 
            redeemable=True)
        
        
        if ticket:
            if user and user.referral:
                available = available.filter(vendor__referral=user.referral)
            else:
                available = available.exclude(vendor__referral__isnull=False)
            
                        
        if ticket:
            available = available.filter(allocate_tickets=True)
        else:
            available = available.filter(allocate_wallet=True)
            if user:
                issued = map(lambda x: x.coupon.pk, filter(lambda x: x.coupon, user.coupon_issue.all()))
                available = available.exclude(pk__in=issued)
        
        if not available:
            return None
        
        if not weighted or len(available) == 1:
            available = list(available)
            return random.choice(available)
        
        # Default Max if NO Limit
        _max = max(available, key=lambda x: x.redeem_limit).redeem_limit
        
        _sum = 0
        for coupon in available:
            _limit = coupon.redeem_limit if coupon.redeem_limit else _max
            if _limit < coupon.issued_redeemed:
                _limit = coupon.issued_redeemed * 5
            _sum += _limit - coupon.issued_redeemed
        
        # Create Odds        
        weight = {}
        for coupon in available:
            _limit = coupon.redeem_limit if coupon.redeem_limit else _max
            if _limit < coupon.issued_redeemed:
                _limit = coupon.issued_redeemed * 5
                
            weight[coupon.pk] = float(_limit - coupon.issued_redeemed) / _sum
            
        # Pick
        # http://stackoverflow.com/a/9260166/153200
        items = weight.keys()
        breakpoint = 0
        breakpoints = [] 
        
        for i in items:
            breakpoint += weight[i]
            breakpoints.append(breakpoint)
        
        score = random.random() * breakpoints[-1]
        i = bisect.bisect(breakpoints, score)
        
        return filter(lambda x: x.pk == items[i], available)[0]
    
    def representation(self):
        return {
            "coupon_id":self.id,
            "title": self.title,
            "image": self.image.name,
            "description": self.description,
            "expires": format_datetime(self.valid_to),
            "redeemed": self.issued_redeemed,
            "redeem_limit":int(self.redeem_limit),
            "remaining": (self.redeem_limit - self.issued_redeemed) if self.redeem_limit else None,
            "geo": True if self.geography else False,
            "vendor": self.vendor.representation() if self.vendor else None
        }
    
    class Meta:
        db_table = u"coupon"
        verbose_name = "Coupon Page"
        #verbose_plural_name = "Coupon Pages"
        
class CouponGeography(models.Model):
    name = models.CharField(max_length=255, unique=True)
    active = models.BooleanField(default=True)
    format = models.CharField(max_length=255)
    
    class Meta:
        db_table = u"coupon_geography"
        
class CouponVote(models.Model):
    coupon = models.ForeignKey(Coupon, related_name="votes")
    user = models.ForeignKey("user.YooLottoUser", related_name="coupon_votes")
    
    # False for Dislike
    # True for Like
    liked = models.BooleanField()
    
    added_at = models.DateTimeField(auto_now_add=True)
    updated_at = models.DateTimeField(auto_now=True)
    
    class Meta:
        db_table = u"coupon_vote"
        unique_together = (("coupon", "user"))
        
class CouponShare(models.Model):
    coupon = models.ForeignKey(Coupon, related_name="shares")
    user = models.ForeignKey("user.YooLottoUser", related_name="coupon_shares")
    type = models.CharField(max_length=255, choices=(
         ("facebook", "facebook"),
         ("twitter", "twitter")
     ))
    
    class Meta:
        db_table = u"coupon_share"

class CouponIssue(models.Model):
    coupon = models.ForeignKey(Coupon, related_name="issued", blank=True, null=True)
    promo = models.ForeignKey(CouponVendorPromo, related_name="issued", blank=True, null=True)
    
    ticket = models.ForeignKey("lottery.LotteryTicket", blank=True, null=True, unique=True, related_name="coupon_issue")
    user = models.ForeignKey("user.YooLottoUser", related_name="coupon_issue")
    
    added_at = models.DateTimeField(auto_now_add=True)
    redeemed = models.DateTimeField(blank=True, null=True)
    
    def representation(self):
        if self.promo:
            return self.promo.representation(user=self.user)
        
        liked = False
        disliked = False
        
        votes = self.coupon.votes.filter(user=self.user)
        if votes:
            vote = votes[0]
            if vote.liked:
                liked = True
            else:
                disliked = True
        
        result = {
            "type": "local",
            "id": self.pk,
            "parent": self.coupon.representation(),
            "redeemed": format_datetime(self.redeemed),
            "added": format_datetime(self.added_at),
            "liked": liked,
            "disliked": disliked
        }
        
        now = datetime.datetime.now()
        if self.redeemed:
            remaining = ((self.redeemed + datetime.timedelta(seconds=COUPON_REDEEM_LIFESPAN)) - now).seconds
            
            if remaining < 0:
                remaining = 0
            
            result["remaining"] = remaining
        
        return result
    
    @classmethod
    def assign(cls, ticket, user=None, weighted=COUPON_WEIGHTED):
#        print "ASSIGN?", user, user.referral
        if ticket and ticket.coupon_issue.exists():
            return ticket.coupon_issue
        
        coupon = None
        if ticket:
            # Temporary Hack for Dickeys Coupons
            if user and user.referral:
#                print "CHOOSING"
                coupon = Coupon.choose(weighted=weighted, ticket=ticket, user=user)
            
            if not coupon:
                coupon = CouponVendorPromo.choose()
        else:
            coupon = Coupon.choose(weighted=weighted, ticket=ticket, user=user)
        
#        print "coupon?", coupon
        
        if not coupon:
            return
        
        # TODO: Transactional Coupons
        _user = ticket.user if ticket else user
        if not _user:
            raise RuntimeError()
        
        if isinstance(coupon, Coupon):
            issue = CouponIssue(coupon=coupon, ticket=ticket, user=_user)
        else:
            issue = CouponIssue(promo=coupon, ticket=ticket, user=_user)
            
        issue.save()
        
        return issue
    
    class Meta:
        db_table = u"coupon_issue"
        
# Signals for Aggregates

@receiver(post_save, sender=CouponIssue)
def update_coupon_from_issue(sender, instance, signal, created, **kwargs):
    if instance.coupon:
        instance.coupon.update_issue(created=created)

@receiver(post_save, sender=CouponVote)
def update_coupon_from_vote(sender, instance, signal, created, **kwargs):
    if instance.coupon:
        instance.coupon.update_votes(created=created)
