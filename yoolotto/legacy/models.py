from __future__ import unicode_literals
from django.db import models

class CouponDislikes(models.Model):
    id = models.BigIntegerField(primary_key=True)
    uid = models.BigIntegerField()
    cid = models.BigIntegerField()
    ctime = models.IntegerField(unique=True)
    class Meta:
        db_table = 'coupon_dislikes'

class CouponLikes(models.Model):
    id = models.BigIntegerField(primary_key=True)
    uid = models.BigIntegerField()
    cid = models.BigIntegerField()
    ctime = models.IntegerField()
    class Meta:
        db_table = 'coupon_likes'

class CouponTokens(models.Model):
    id = models.BigIntegerField(primary_key=True)
    cid = models.IntegerField()
    uid = models.BigIntegerField()
    key = models.CharField(max_length=255L)
    ctime = models.IntegerField()
    shown = models.IntegerField(null=True, blank=True)
    stime = models.IntegerField()
    redeemed = models.IntegerField(null=True, blank=True)
    rtime = models.IntegerField()
    liked = models.IntegerField(null=True, blank=True)
    lktime = models.IntegerField()
    disliked = models.IntegerField(null=True, blank=True)
    dktime = models.IntegerField()
    class Meta:
        db_table = 'coupon_tokens'

class Coupons(models.Model):
    id = models.BigIntegerField(primary_key=True)
    name = models.CharField(max_length=100L)
    title = models.CharField(max_length=255L)
    description = models.TextField()
    data = models.TextField()
    barcode = models.BigIntegerField()
    ctime = models.IntegerField()
    status = models.CharField(max_length=8L)
    starttime = models.IntegerField(db_column='startTime') # Field name made lowercase.
    endtime = models.IntegerField(db_column='endTime') # Field name made lowercase.
    class Meta:
        db_table = 'coupons'

class Drawings(models.Model):
    id = models.BigIntegerField(primary_key=True)
    drawid = models.BigIntegerField()
    type = models.CharField(max_length=12L)
    state = models.CharField(max_length=5L)
    winnings = models.CharField(max_length=255L, blank=True)
    dtime = models.IntegerField()
    rtime = models.IntegerField()
    class Meta:
        db_table = 'drawings'

class Drawings2(models.Model):
    id = models.BigIntegerField(primary_key=True)
    drawid = models.BigIntegerField()
    type = models.CharField(max_length=12L)
    state = models.CharField(max_length=5L)
    winnings = models.CharField(max_length=255L, blank=True)
    dtime = models.IntegerField()
    rtime = models.IntegerField()
    class Meta:
        db_table = 'drawings2'

class FbTokens(models.Model):
    id = models.BigIntegerField(primary_key=True)
    fbid = models.BigIntegerField()
    access_token = models.TextField()
    status = models.CharField(max_length=7L)
    class Meta:
        db_table = 'fb_tokens'

class FetchCouponStats(models.Model):
    id = models.BigIntegerField(primary_key=True)
    uid = models.BigIntegerField()
    response_url = models.TextField()
    ctime = models.IntegerField()
    class Meta:
        db_table = 'fetch_coupon_stats'

class FetchTicketsStats(models.Model):
    id = models.BigIntegerField(primary_key=True)
    uid = models.BigIntegerField()
    ctime = models.IntegerField()
    class Meta:
        db_table = 'fetch_tickets_stats'

class Fetches(models.Model):
    id = models.BigIntegerField(primary_key=True)
    cid = models.IntegerField()
    uid = models.BigIntegerField()
    key = models.CharField(max_length=255L)
    ctime = models.IntegerField()
    shown = models.IntegerField(null=True, blank=True)
    stime = models.IntegerField()
    redeemed = models.IntegerField(null=True, blank=True)
    rtime = models.IntegerField()
    class Meta:
        db_table = 'fetches'

class Promomay2013(models.Model):
    id = models.BigIntegerField(primary_key=True)
    email = models.CharField(max_length=100L, unique=True)
    ip = models.CharField(max_length=15L)
    ctime = models.IntegerField()
    game = models.CharField(max_length=100L)
    confirmation = models.CharField(max_length=100L)
    class Meta:
        db_table = 'promomay2013'

class Tickets(models.Model):
    id = models.BigIntegerField(primary_key=True)
    uid = models.BigIntegerField()
    tid = models.CharField(max_length=255L)
    serial = models.BigIntegerField()
    type = models.CharField(max_length=12L)
    state = models.CharField(max_length=5L)
    rows = models.TextField()
    drawnumber = models.BigIntegerField(db_column='drawNumber') # Field name made lowercase.
    dtime = models.IntegerField()
    drawtime = models.CharField(max_length=50L)
    ctime = models.IntegerField()
    ctime2 = models.DateTimeField()
    class Meta:
        db_table = 'tickets'

class Users(models.Model):
    id = models.BigIntegerField(primary_key=True)
    fbid = models.BigIntegerField()
    deviceid = models.CharField(max_length=255L, unique=True)
    key = models.CharField(max_length=128L)
    email = models.CharField(max_length=100L, blank=True)
    name = models.CharField(max_length=255L)
    os = models.CharField(max_length=7L, blank=True)
    ctime = models.IntegerField()
    ctime2 = models.DateTimeField()
    class Meta:
        db_table = 'users'

class Winnings(models.Model):
    id = models.BigIntegerField(primary_key=True)
    tid = models.BigIntegerField(unique=True)
    data = models.TextField()
    total = models.BigIntegerField()
    ctime = models.IntegerField()
    class Meta:
        db_table = 'winnings'


class ImportDrawingLinkage(models.Model):
    legacy = models.ForeignKey(Drawings)
    draw = models.ForeignKey("lottery.LotteryDraw")
    
    class Meta:
        db_table = "_migration_drawings_lottery_draw"
        
class ImportDrawing2Linkage(models.Model):
    legacy = models.ForeignKey(Drawings2)
    draw = models.ForeignKey("lottery.LotteryDraw")
    
    class Meta:
        db_table = "_migration_drawings2_lottery_draw"