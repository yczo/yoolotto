import json
from decimal import Decimal
import datetime
from django.db import models
from django.db import connection
from django.db.models.signals import post_save
from django.dispatch import receiver

from yoolotto.coin.models import CoinTicketTransaction, CoinTransaction
from yoolotto.coin.manager import CoinTicketManager
from yoolotto.coupon.models import Coupon, CouponIssue
from yoolotto.lottery import representations

class LotteryCountryDivision(models.Model):
    name = models.CharField(max_length=255)
    
    remote_id = models.CharField(max_length=8, unique=True)
    remote_country = models.CharField(max_length=32)
    
    def __unicode__(self):
        return self.remote_id
    
    class Meta:
        db_table = u"lottery_country_division"
        
class LotteryGame(models.Model):
    name = models.CharField(max_length=255)
    code = models.CharField(max_length=64, unique=True)
    active = models.BooleanField(default=True)
    
    def __unicode__(self):
        return u"id(%s)-name(%s)-code(%s)"%(self.id,self.name,self.code)

    @property
    def handler(self):
        from yoolotto.lottery.game.manager import GameManager
        return GameManager.get(self.code)
    
    def representation(self, user=None):
        from yoolotto.lottery.enumerations import EnumerationManager
        result = {
            "name": self.name,
            "active": self.active,
            "components": map(lambda x: x.representation(user=user), self.components.all()),
            "id": self.pk,
            "gameType": EnumerationManager.game_reverse(self.pk)
        }
        
        if user:
            tickets = user.tickets.filter(draw__component__parent=self)
            plays = sum(map(lambda x: x.plays.count(), tickets))
            ticket_record = LotteryTicket.objects.filter(user_id = user)[0]
            ticket_available = LotteryTicketAvailable.objects.filter(ticket_id = ticket_record.id)
            result["tickets"] = tickets.count()
            result["plays"] = plays
            
            tickets = user.tickets.filter(draw__component__parent=self, draw__result__isnull=False)
                             
            # Game Checkable
            result["checkable"] = True if sum(map(lambda x: 1 if not x.all_checked else 0, tickets)) > 0 else False
            print "checkable",result["checkable"]
            #print "result chekable",result["checkable"]
            # Unchecked Plays
            result["unchecked"] = sum(map(lambda x: x.unchecked, tickets))
            print "result",result["unchecked"]
            #print "draw",ticket_record.draw.result
            print "ticket_id",ticket_record.id
            if not result["checkable"] and ticket_record.draw.result:
                result["gameState"] = 1
            elif not result["checkable"] and not ticket_record.draw.result:
                result["gameState"] = 0
            elif result["checkable"] and ticket_record.draw.result and not ticket_available[0].pending:
                result["gameState"] = 2
            elif not ticket_available[0].available and not ticket_available[0].rejected:
                result["gameState"] = 4
            elif not ticket_available[0].rejected:
                result["gameState"] = 3
            
            print "gamestate",result["gameState"]
            '''try:
            if not self.ticket.all_checked and not self.ticket.draw.result:
                _result["gameState"] = 0
            elif not self.ticket.all_checked and self.ticket.draw.result :
                _result["gameState"] = 1
            elif self.ticket.all_checked and self.ticket.draw.result and not self.ticket.ticket_submissions.all()[0].pending:
                _result["gameState"] = 2
            elif self.ticket.ticket_submissions.all()[0].rejected:
                _result["gameState"] = 3
            elif not self.ticket.ticket_submissions.all()[0].available and not self.ticket.ticket_submissions.all()[0].rejected:
                _result["gameState"] = 4
        except:
            pass'''
            
        return result
    
    class Meta:
        db_table = u"lottery_game"

class LotteryGameComponent(models.Model):
    name = models.CharField(max_length=255)
    
    parent = models.ForeignKey(LotteryGame, blank=True, null=True, related_name="components")
    #remote_id = models.CharField(max_length=8, unique=True)
    remote_id = models.CharField(max_length=8)
    
    active = models.BooleanField(default=True)
    division = models.ManyToManyField(LotteryCountryDivision, related_name="components")
    
    # Defines the "format" used by the parent.handler
    # This is primarily used when the results of a game are made in separate 
    # draws (i.e. MEGA Millions)
    format = models.CharField(max_length=255, blank=True, null=True)
    
    # Defines an unique identifier for the component
    identifier = models.CharField(max_length=255, blank=True, null=True)
    
    def __unicode__(self):
        return self.name

    def representation(self, user=None):
        # Retrieve Latest Drawings
        draws = []
        
        for i, draw in enumerate(self.draws.filter(official=True).order_by("-date")[:2]):
#            _draw = {
#                "id": draw.id,
#                "future": True if i == 0 else False,
#                "jackpot": draw.jackpot,
#                "date": draw.date.strftime("%Y-%m-%d"),
#                "result": json.loads(draw.result) if draw.result else None
#            }
            
            _draw = draw.representation()
            
            if user:
                ticket = user.tickets.filter(draw=draw)
                if not ticket:
                    _draw["checkable"] = False
                    _draw["uncheckedPlayCount"] = 0
                else:
                    ticket = ticket[0]
                    _draw["checkable"] = True if draw.result else False
                    _draw["uncheckedPlayCount"] = ticket.unchecked
            
            draws.append(_draw)
                
        result = {
            "id": self.id,
            "active": True if self.active and self.format else False,
            "name": self.name,
            "draws": draws
        }
        
        result.update(self.parent.handler.get_game_meta(self))
        return result
    
    def __str__(self):
        return "<%s id: %s name:%s>" % (self.__class__.__name__, self.pk, self.name)
    
    class Meta:
        db_table = u"lottery_game_component"
        
class LotteryDraw(models.Model):
    component = models.ForeignKey(LotteryGameComponent, related_name="draws")
    date = models.DateField()
    
    jackpot = models.IntegerField(blank=True, null=True)
    result = models.CharField(max_length=255, blank=True, null=True)
    division = models.ForeignKey(LotteryCountryDivision,null=True,blank=False)
    official = models.BooleanField(default=False)
    frenzied = models.BooleanField(default=False)
    # Powerball
    powerplay = models.IntegerField(default=0, blank=True, null=True)
    five_of_five_only = models.IntegerField(default=0,blank=True, null=True)
    five_of_five_powerplay = models.IntegerField(default=0,blank=True, null=True)
    four_of_five_powerball = models.IntegerField(default=0,blank=True, null=True)
    four_of_five_only = models.IntegerField(default=0,blank=True, null=True)
    three_of_five_with_powerball = models.IntegerField(default=0,blank=True, null=True)
    three_of_five_only = models.IntegerField(default=0,blank=True, null=True)
    two_of_five_powerball = models.IntegerField(default=0,blank=True, null=True)
    two_of_five_only = models.IntegerField(default=0,blank=True, null=True)
    one_of_five_powerball = models.IntegerField(default=0,blank=True, null=True)
    powerball_only = models.IntegerField(default=0,blank=True, null=True)

    # For MegaMillion only
    megaplier = models.IntegerField(default=0,blank=True, null=True)
    megaball = models.IntegerField(default=0,blank=True, null=True)
    four_of_five_megaball = models.IntegerField(default=0,blank=True, null=True)
    three_of_five_with_megaball = models.IntegerField(default=0,blank=True, null=True)
    two_of_five_megaball = models.IntegerField(default=0,blank=True, null=True)
    one_of_five_megaball = models.IntegerField(default=0,blank=True, null=True)

    # for lottoTX
    extra = models.IntegerField(default=0,blank=True, null=True)
    six_of_six_only = models.IntegerField(default=0,blank=True, null=True)
    five_of_six_only = models.IntegerField(default=0,blank=True, null=True)
    five_of_six_extra = models.IntegerField(default=0,blank=True, null=True)
    four_of_six_only = models.IntegerField(default=0,blank=True, null=True)
    four_of_six_extra = models.IntegerField(default=0,blank=True, null=True)
    three_of_six_only = models.IntegerField(default=0,blank=True, null=True)
    three_of_six_extra = models.IntegerField(default=0,blank=True, null=True)
    two_of_six_only = models.IntegerField(default=0,blank=True, null=True)
    two_of_six_extra = models.IntegerField(default=0,blank=True, null=True)

    #Two Step
    bonus = models.IntegerField(default=0,blank=True, null=True)
    four_of_four = models.IntegerField(default=0,blank=True, null=True)
    four_of_four_bonus = models.IntegerField(default=0,blank=True, null=True)
    three_of_four = models.IntegerField(default=0,blank=True, null=True)
    three_of_four_bonus = models.IntegerField(default=0,blank=True, null=True)
    two_of_four = models.IntegerField(default=0,blank=True, null=True)
    two_of_four_bonus = models.IntegerField(default=0,blank=True, null=True)
    one_of_four = models.IntegerField(default=0,blank=True, null=True)
    one_of_four_bonus = models.IntegerField(default=0,blank=True, null=True)

    # For Daily3 and Daily4
    straight = models.IntegerField(default=0,blank=True, null=True) 
    box = models.IntegerField(default=0,blank=True, null=True)
    staright_and_box = models.IntegerField(default=0,blank=True, null=True)
    box_only = models.IntegerField(default=0,blank=True, null=True)

    # For Daily Derby
    win = models.IntegerField(default=0,blank=True, null=True)
    exacta = models.IntegerField(default=0,blank=True, null=True)
    trifecta = models.IntegerField(default=0,blank=True, null=True)
    race_time = models.CharField(max_length=80,blank=True,null=True)
    exacta_with_racetime = models.IntegerField(default=0,blank=True, null=True)
    win_with_racetime = models.IntegerField(default=0,blank=True, null=True)
    race_time_amount = models.IntegerField(default=0,blank=True, null=True)

    #For All or Nothing
    twelve_of_twelve = models.IntegerField(default=0,blank=True, null=True)
    eleven_of_twelve = models.IntegerField(default=0,blank=True, null=True)
    ten_of_tweleve = models.IntegerField(default=0,blank=True, null=True)
    nine_of_twelve = models.IntegerField(default=0,blank=True, null=True)
    eight_of_twelve = models.IntegerField(default=0,blank=True, null=True)
    four_of_twelve = models.IntegerField(default=0,blank=True, null=True)
    three_of_twelve = models.IntegerField(default=0,blank=True, null=True)
    two_of_twelve = models.IntegerField(default=0,blank=True, null=True)
    one_of_twelve = models.IntegerField(default=0,blank=True, null=True)
    zero_of_twelve = models.IntegerField(default=0,blank=True, null=True)


    updated = models.DateTimeField(null=True,blank=True)
    number_of_winners = models.IntegerField(default=0,blank=True, null=True)
    odds = models.CharField(max_length=255, blank=True, null=True)
    
    def representation(self, ticket=None):
        
        if self.component.parent.code == "DailyDerby":
            try:
                result = {
                    "id": self.id,
                    "jackpot": self.jackpot,
                    "date": self.date.strftime("%Y-%m-%d"),
                    "future": True if not self.result else False,
                    "result": json.loads(self.result) if self.result else None,
                    "result_race_time":json.loads(self.race_time.replace(':','.').replace('.',':',1)) if self.race_time else None,
                }
            except:
                earning_obj = None
                result = {
                    "id": self.id,
                    "jackpot": self.jackpot,
                    "date": self.date.strftime("%Y-%m-%d"),
                    "future": True if not self.result else False,
                    "result": json.loads(self.result) if self.result else None,
                    "result_race_time":None
                }
            
        else:
            result = {
            "id": self.id,
            "jackpot": self.jackpot,
            "date": self.date.strftime("%Y-%m-%d"),
            "future": True if not self.result else False,
            "result": json.loads(self.result) if self.result else None,
        }
        
        if self.component.identifier in ["DailyFourDay", "DailyFourNight", "DailyFourMorning", "DailyFourEvening"]:
            if result["result"] and len(result["result"]) == 4:
                _sum = sum(result["result"])
                result["result"] += [_sum]
        
        if self.component.identifier in ["PickThreeDay", "PickThreeNight", "PickThreeMorning", "PickThreeEvening"]:
            if result["result"] and len(result["result"]) == 3:
                _sum = sum(result["result"])
                result["result"] += [_sum]
                
        if self.component.identifier in ["MegaMillions"]:
            # Find Megaplier
            try:
                megaplier = LotteryDraw.objects.get(date=self.date, component__identifier="Megaplier",division=self.division)
                if result["result"]:
                    if megaplier.result: 
                        result["result"] += json.loads(megaplier.result)
                    else:
                        result["result"].append(None)
            except LotteryDraw.DoesNotExist:
                if result["result"]:
                    result["result"].append(None)

        if ticket:
            result["uncheckedPlayCount"] = ticket.unchecked
            result["checkable"] = ticket.checkable
        
        #if not result['jackpot']:
        #    ahead_date = self.date
        #    draw = LotteryDraw.objects.filter(date__gte=datetime.datetime.now().today(),\
        #                                        component=self.component,jackpot__gt=0)
        #    try:
        #        result['jackpot'] = draw[0].jackpot
        #    except Exception as e:
        #        print e
        return result
        
    class Meta:
        db_table = u"lottery_draw"
        
class LotteryDrawFrenzy(models.Model):
    draw = models.ForeignKey(LotteryDraw, related_name="frenzies")
    added_at = models.DateTimeField(auto_now_add=True)
    
    class Meta:
        db_table = u"lottery_draw_frenzy"
        
   
class LotteryTicket(models.Model):
    draw = models.ForeignKey(LotteryDraw, related_name="tickets")
    user = models.ForeignKey("user.YooLottoUser",related_name="tickets")
    division = models.ForeignKey(LotteryCountryDivision,null=True,blank=False)
    winnings = models.FloatField(default = 0)
    checked = models.BooleanField(default=False)
    notified = models.BooleanField(default=False)
    deleted = models.BooleanField(default=False)    
    added_at = models.DateTimeField(auto_now_add=True)
    fantasy = models.BooleanField(default = False)
    winnings_coins = models.FloatField(default = 0)
    def coupon_representation(self):
        if not self.coupon_issue.exists():
            return None
        
        return self.coupon_issue.get().representation()
    
    def coin_representation(self):
        transactions = []
        
        transactions.extend(self.coin_ticket_transaction.all())
        
        for submission in self.submissions.all():
            transactions.extend(submission.transaction_submission.all())
        
        result = sum([x.transaction.amount for x in transactions])
        
        return result if result > 0 else None
            
    def update(self, device=None, Client_login=None, full=False):
        if full:
            self.calculate_winnings()
        
        allocated = self.update_coins(device, Client_login, save=False)
        self.update_coupons(save=False)
        self.update_winnings(save=False)
        
        self.save()
                
        return allocated
        
    def update_coins(self, device, Client_login, save=False):
        manager = CoinTicketManager(device, Client_login, ticket=self)
        manager.update1()
        
        if save:
            self.save()
        
        return 0
    
    def update_coupons(self, save=False):
        if not self.draw.result:
            return
        
        if self.coupon_issue.exists():
            return
                
        CouponIssue.assign(self, user=self.user)
    
    def update_winnings(self, save=True):
        if not self.draw.result:
            self.winnings = None
            if save:
                save.save()
            
            return
        if self.fantasy == False:
            total = Float("0.00")
            result = 0
                    
            for play in self.plays.all():
                if play.winnings is None:
                    continue
                total += play.winnings
                result += 1
            
            if result == 0:
                self.winnings = None
            else:
                self.winnings = total
                    
            if save:
                self.save()
            
    def calculate_winnings(self):
        map(lambda x: x.update_winnings(), self.plays.exclude(play=None))

    def representation1(self, 
        representation=representations.TicketRepresentationShortList1,
        *args, **kwargs):
        return representation(self)(*args, **kwargs)
            
    def representation(self, 
        representation=representations.TicketRepresentationShortList,
        *args, **kwargs):
        return representation(self)(*args, **kwargs)
    
    @property
    def unchecked(self):
        cursor = connection.cursor()
        
        query = """
        SELECT COUNT(1)
        FROM lottery_ticket_submission
        LEFT OUTER JOIN lottery_ticket_play ON lottery_ticket_play.submission_id = lottery_ticket_submission.id
        WHERE lottery_ticket_submission.ticket_id = %s
        AND checked = 0;
        """
        
        cursor.execute(query, [self.pk,])
        return int(cursor.fetchone()[0])
                    
    @property
    def checkable(self):
        return True if self.draw.result else False
    
    @property
    def checked(self):
        raise RuntimeError("Property Deprecated")
    
    @property
    def all_checked(self):
        return False if sum([True if not x["checked"] else False for x in self.submissions.all().values("checked")]) > 0 else True

    class Meta:
        db_table = u"lottery_ticket"

class LotteryTicketClient(models.Model):
    draw = models.ForeignKey(LotteryDraw, related_name="tickets_client")
    device = models.CharField(max_length=255, blank=True, null=True)
    email = models.CharField(max_length=255, blank=True, null=True)
    added_at = models.DateTimeField(auto_now_add=True)

    class Meta:
        db_table = u"lottery_ticket_client"

        
        
class LotteryTicketSubmission(models.Model):
    """
    Separator for physical tickets
    
    """
    submission = models.CharField(max_length=16, blank=True, null=True)
    ticket = models.ForeignKey(LotteryTicket, related_name="submissions")
    checked = models.BooleanField(default=False)
    added_at = models.DateTimeField(auto_now_add=True)
    
    class Meta:
        db_table = u"lottery_ticket_submission"

class LotteryTicketPlay(models.Model):
    ticket = models.ForeignKey(LotteryTicket, related_name="plays")
    play = models.CharField(max_length=255, blank=True, null=True)
    division = models.ForeignKey(LotteryCountryDivision,null=True,blank=False)
    submission_old = models.CharField(max_length=16, blank=True, null=True, db_column="submission")
    submission = models.ForeignKey(LotteryTicketSubmission, related_name="plays", blank=True, null=True)
    
    winnings = models.FloatField(default = 0)
    
    winnings_base = models.DecimalField(max_digits=15, decimal_places=2, blank=True, null=True)
    winnings_sum = models.DecimalField(max_digits=15, decimal_places=2, blank=True, null=True)
    
    added_at = models.DateTimeField(auto_now_add=True)
    winnings_coins = models.FloatField(default = 0)
    
    def calculate_fantasyfive_winnings(self, winnings, draw, game):
        """
        This will return the winning amount as per request
        """
        if winnings == game.FIVE_OF_FIVE:
            winnings = draw.five_of_five_only

        if winnings == game.FOUR_OF_FIVE:
            winnings = draw.four_of_five_only

        if winnings == game.THREE_OF_FIVE:
            winnings = draw.three_of_five_only

        if winnings == game.TWO_OF_FIVE:
            winnings = draw.two_of_five_only

        return winnings

    def calculate_dailythree_winnings(self, winnings, draw, game):
        
        if winnings == game.STRAIGHT:
            winnings = draw.straight

        if winnings == game.STRAIGHT_AND_BOX:
            winnings = draw.staright_and_box

        if winnings == game.BOX:
            winnings = draw.box

        if winnings == game.BOX_ONLY:
            winnings = draw.box_only
        return winnings

    def calculate_dailyfor_winnings(self, winnings, draw, game):

        if winnings == game.STRAIGHT:
            winnings = draw.straight

        if winnings == game.STRAIGHT_AND_BOX:
            winnings = draw.staright_and_box

        if winnings == game.BOX:
            winnings = draw.box

        if winnings == game.BOX_ONLY:
            winnings = draw.box_only
        
        return winnings

    def calculate_superlottoplus_winnings(self, winnings, draw, game):
        """
        This will return the winning amount as per request
        """
   
        if winnings == game.ONE_OF_FIVE_MEGABALL:
            winnings = draw.one_of_five_megaball

        if winnings == game.TWO_OF_FIVE_MEGABALL:
            winnings = draw.two_of_five_megaball

        if winnings == game.THREE_OF_FIVE_MEGABALL:
            winnings = draw.three_of_five_with_megaball

        if winnings == game.FOUR_OF_FIVE_MEGABALL:
            winnings = draw.four_of_five_megaball

        if winnings == game.MEGABALL_ONLY:
            winnings = draw.megaball                
        
        if winnings == game.FIVE_OF_FIVE:
            winnings = draw.five_of_five_only

        if winnings == game.FOUR_OF_FIVE:
            winnings = draw.four_of_five_only

        if winnings == game.THREE_OF_FIVE:
            winnings = draw.three_of_five_only

        if winnings == game.TWO_OF_FIVE:
            winnings = draw.two_of_five_only
        return winnings

    def calculate_dailyderby_winnings(self, winnings, draw, game):
        """
        This will return the winning amount as per request
        """
        
        if winnings == game.TRIFECTA:
            winnings = draw.trifecta

        if winnings == game.EXACTA:
            winnings = draw.exacta

        if winnings == game.RACETIME_ONLY:
            winnings = draw.race_time_amount
        
        if winnings == game.EXACTA_WITH_RACETIME:
            winnings = draw.exacta_with_racetime

        if winnings == game.WIN:
            winnings = draw.win

        if winnings == game.WIN_WITH_RACETIME:
            winnings = draw.win_with_racetime

        return winnings
    
    def calculate_megamillionca_winnings(self, winnings, draw, game):
        
        if winnings == game.ONE_OF_FIVE_MEGABALL:
            winnings = draw.one_of_five_megaball

        if winnings == game.TWO_OF_FIVE_MEGABALL:
            winnings = draw.two_of_five_megaball

        if winnings == game.THREE_OF_FIVE_MEGABALL:
            winnings = draw.three_of_five_with_megaball

        if winnings == game.FOUR_OF_FIVE_MEGABALL:
            winnings = draw.four_of_five_megaball

        if winnings == game.MEGABALL_ONLY:
            winnings = draw.megaball                
        
        if winnings == game.FIVE_OF_FIVE:
            winnings = draw.five_of_five_only

        if winnings == game.FOUR_OF_FIVE:
            winnings = draw.four_of_five_only

        if winnings == game.THREE_OF_FIVE:
            winnings = draw.three_of_five_only

        if winnings == game.TWO_OF_FIVE:
            winnings = draw.two_of_five_only

        # if winnings == game.MEGABALL_MEGAPLIER:
        #     winnings = draw.megaball * draw.megaplier

        # if winnings == game.ONE_OF_FIVE_MEGABALL_MEGAPLIER:
        #     winnings = draw.one_of_five_megaball * draw.megaplier

        # if winnings == game.TWO_OF_FIVE_MEGABALL_MEGAPLIER:
        #     winnings = draw.two_of_five_megaball * draw.megaplier

        # if winnings == game.THREE_OF_FIVE_MEGABALL_MEGAPLIER:
        #     winnings = draw.three_of_five_with_megaball * draw.megaplier

        # if winnings == game.THREE_OF_FIVE_MEGAPLIER:
        #     winnings = draw.three_of_five_only * draw.megaplier

        # if winnings == game.FOUR_OF_FIVE_MEGABALL_MEGAPLIER:
        #     winnings = draw.four_of_five_megaball * draw.megaplier

        # if winnings == game.FOUR_OF_FIVE_MEGAPLIER:
        #     winnings = draw.four_of_five_only * draw.megaplier

        # if winnings == game.FIVE_OF_FIVE_MEGAPLIER:
        #     winnings = draw.five_of_five_only * draw.megaplier
        return winnings
    
    def calculate_megamillion_winnings(self, winnings, draw, game):
        """
        This will return the winning amount as per request
        """
       
        component = LotteryGameComponent.objects.get(parent__code=game.NAME, format="Megaplier")
        try:
            megaplier = LotteryDraw.objects.get(date=draw.date,
                                                component=component,
                                                official=True)
            megaplier = json.loads(megaplier.result)[0]
        except Exception as e:
            megaplier = 1
        
        if winnings == game.ONE_OF_FIVE_MEGABALL:
            winnings = draw.one_of_five_megaball

        if winnings == game.TWO_OF_FIVE_MEGABALL:
            winnings = draw.two_of_five_megaball

        if winnings == game.THREE_OF_FIVE_MEGABALL:
            winnings = draw.three_of_five_with_megaball

        if winnings == game.FOUR_OF_FIVE_MEGABALL:
            winnings = draw.four_of_five_megaball

        if winnings == game.MEGABALL_ONLY:
            winnings = draw.megaball                
        
        if winnings == game.FIVE_OF_FIVE:
            winnings = draw.five_of_five_only

        if winnings == game.FOUR_OF_FIVE:
            winnings = draw.four_of_five_only

        if winnings == game.THREE_OF_FIVE:
            winnings = draw.three_of_five_only

        if winnings == game.TWO_OF_FIVE:
            winnings = draw.two_of_five_only

        if winnings == game.MEGABALL_MEGAPLIER:
            winnings = draw.megaball * megaplier

        if winnings == game.ONE_OF_FIVE_MEGABALL_MEGAPLIER:
            winnings = draw.one_of_five_megaball * megaplier

        if winnings == game.TWO_OF_FIVE_MEGABALL_MEGAPLIER:
            winnings = draw.two_of_five_megaball * megaplier

        if winnings == game.THREE_OF_FIVE_MEGABALL_MEGAPLIER:
            winnings = draw.three_of_five_with_megaball * megaplier

        if winnings == game.THREE_OF_FIVE_MEGAPLIER:
            winnings = draw.three_of_five_only * megaplier

        if winnings == game.FOUR_OF_FIVE_MEGABALL_MEGAPLIER:
            winnings = draw.four_of_five_megaball * megaplier

        if winnings == game.FOUR_OF_FIVE_MEGAPLIER:
            winnings = draw.four_of_five_only * megaplier

        if winnings == game.FIVE_OF_FIVE_MEGAPLIER:
            winnings = draw.five_of_five_only * megaplier

        return winnings

    def calculate_fantasymegamillion_winnings(self,_winnings):
        play_result = _winnings

        if play_result == "MEGABALL_ONLY":
            winnings = "1@"

        if play_result == "ONE_OF_FIVE_MEGABALL":
            winnings = "2@"

        if play_result == "TWO_OF_FIVE_MEGABALL":
            winnings = "5@"

        if play_result == "THREE_OF_FIVE":
            winnings = "5@"

        if play_result == "THREE_OF_FIVE_MEGABALL":
            winnings = "50@"

        if play_result == "FOUR_OF_FIVE":
            winnings = "10$"

        if play_result == "FOUR_OF_FIVE_MEGABALL":
            winnings = "500$"

        if play_result == "FIVE_OF_FIVE":
            winnings = "5000$"

        if play_result == "JACKPOT":
            winnings = "1000000$"

        return winnings

    def calculate_fantasypowerball_winnings(self,_winnings):
        print "in calculate_fantasypowerball_winnings"
        play_result = _winnings
        
        if play_result == "POWERBALL_ONLY":
            winnings = "4@"

        if play_result == "ONE_OF_FIVE_POWERBALL":
            winnings = "4@"

        if play_result == "TWO_OF_FIVE_POWERBALL":
            winnings = "7@"

        if play_result == "THREE_OF_FIVE":
            winnings = "7@"

        if play_result == "THREE_OF_FIVE_POWERBALL":
            winnings = "100@"

        if play_result == "FOUR_OF_FIVE":
            winnings = "10$"

        if play_result == "FOUR_OF_FIVE_POWERBALL":
            winnings = "500$"

        if play_result == "FIVE_OF_FIVE":
            winnings = "5000$"

        if play_result == "JACKPOT":
            winnings = "1000000$"

        return winnings

    def calculate_powerball_winnings(self, winnings, draw, game):
        """
        This will return the winning amount as per request
        """
        component = LotteryGameComponent.objects.get(parent__code=game.NAME, format="Powerplay")
        try:
            _powerplay = LotteryDraw.objects.get(date=draw.date,
                                            component=component,
                                            official=True)
            _powerplay = json.loads(_powerplay.result)[0]
        
        except Exception as e:
            print e
            _powerplay = 1
        if winnings == game.FOUR_OF_FIVE:
            winnings = draw.four_of_five_only

        if winnings == game.FOUR_OF_FIVE_POWERBALL:
            winnings = draw.four_of_five_powerball

        if winnings == game.FOUR_OF_FIVE_POWERPLAY:
            winnings = draw.four_of_five_only * _powerplay

        if winnings == game.FOUR_OF_FIVE_POWERBALL_POWERPLAY:
            winnings = draw.four_of_five_powerball * _powerplay

        if winnings == game.THREE_OF_FIVE:
            winnings = draw.three_of_five_only

        if winnings == game.THREE_OF_FIVE_POWERBALL:
            winnings = draw.three_of_five_with_powerball

        if winnings == game.THREE_OF_FIVE_POWERPLAY:
            winnings = draw.three_of_five_only * _powerplay

        if winnings == game.THREE_OF_FIVE_POWERBALL_POWERPLAY:
            winnings = draw.three_of_five_with_powerball * _powerplay

        if winnings == game.TWO_OF_FIVE:
            winnings = draw.two_of_five_only

        if winnings == game.TWO_OF_FIVE_POWERBALL:
            winnings = draw.two_of_five_powerball

        if winnings == game.TWO_OF_FIVE_POWERBALL_POWERPLAY:
            winnings = draw.two_of_five_powerball * _powerplay

        if winnings == game.ONE_OF_FIVE:
            winnings = draw.one_of_five_only

        if winnings == game.ONE_OF_FIVE_POWERBALL:
            winnings = draw.one_of_five_powerball

        if winnings == game.ONE_OF_FIVE_POWERBALL_POWERPLAY:
            winnings = draw.one_of_five_powerball * _powerplay

        if winnings == game.POWERBALL_ONLY:
            winnings = draw.powerball_only

        if winnings == game.POWERBALL_ONLY_POWERPLAY:
            winnings = draw.powerball_only * _powerplay

        if winnings == game.FIVE_OF_FIVE:
            winnings = draw.five_of_five_only

        if winnings == game.FIVE_OF_FIVE_POWERPLAY:
            winnings = draw.five_of_five_only * 2

        return winnings


    def calculate_powerballca_winnings(self, winnings, draw, game):
        """
        This will return the winning amount as per request
        """

        if winnings == game.FOUR_OF_FIVE:
            winnings = draw.four_of_five_only

        if winnings == game.FOUR_OF_FIVE_POWERBALL:
            winnings = draw.four_of_five_powerball
        
        if winnings == game.THREE_OF_FIVE:
            winnings = draw.three_of_five_only

        if winnings == game.THREE_OF_FIVE_POWERBALL:
            winnings = draw.three_of_five_with_powerball
        
        if winnings == game.TWO_OF_FIVE:
            winnings = draw.two_of_five_only

        if winnings == game.TWO_OF_FIVE_POWERBALL:
            winnings = draw.two_of_five_powerball
        
        if winnings == game.ONE_OF_FIVE:
            winnings = draw.one_of_five_only

        if winnings == game.ONE_OF_FIVE_POWERBALL:
            winnings = draw.one_of_five_powerball

        if winnings == game.POWERBALL_ONLY:
            winnings = draw.powerball_only
        
        if winnings == game.FIVE_OF_FIVE:
            winnings = draw.five_of_five_only

        return winnings

    def calculate_allornothing_winnings(self, winnings, draw, game):
        if winnings == game.TWELVE_OF_TWELVE:
            winnings = draw.twelve_of_twelve

        if winnings == game.ELEVEN_OF_TWELVE:
            winnings = draw.eleven_of_twelve

        if winnings == game.TEN_OF_TWELVE:
            winnings = draw.ten_of_tweleve
        
        if winnings == game.NINE_OF_TWELVE:
            winnings = draw.nine_of_twelve

        if winnings == game.EIGHT_OF_TWELVE:
            winnings = draw.eight_of_twelve

        return winnings
    
    def calculate_twostep_winnings(self, winnings, draw, game):
        
        if winnings == game.FOUR_OF_FOUR_BONUS:
            winnings = draw.four_of_four_bonus

        if winnings == game.THREE_OF_FOUR_BONUS:
            winnings = draw.three_of_four_bonus

        if winnings == game.THREE_OF_FOUR:
            winnings = draw.three_of_four
        
        if winnings == game.TWO_OF_FOUR_BONUS:
            winnings = draw.two_of_four_bonus

        if winnings == game.ONE_OF_FOUR_BONUS:
            winnings = draw.one_of_four_bonus

        if winnings == game.ZERO_OF_BONUS:
            winnings = draw.bonus
        
        if winnings == game.FOUR_OF_FOUR:
            winnings = draw.four_of_four
        
        return winnings

    def calculate_lotto_winnings(self, winnings, draw, game):
        
        if winnings == game.SIX_OF_SIX:
            winnings = draw.six_of_six_only

        if winnings == game.FIVE_OF_SIX:
            winnings = draw.five_of_six_only

        if winnings == game.FOUR_OF_SIX:
            winnings = draw.four_of_six_only
        
        if winnings == game.THREE_OF_SIX:
            winnings = draw.three_of_six_only

        if winnings == game.TWO_OF_SIX:
            winnings = draw.two_of_six_only

        if winnings == game.FIVE_OF_SIX_EXTRA:
            winnings = draw.five_of_six_extra

        if winnings == game.FOUR_OF_SIX_EXTRA:
            winnings = draw.four_of_six_extra

        if winnings == game.THREE_OF_SIX_EXTRA:
            winnings = draw.three_of_six_extra

        if winnings == game.TWO_OF_SIX_EXTRA:
            winnings = draw.two_of_six_extra

        return winnings

    def fantasy_earnings(self,play,game_type):
        draw = play.ticket.draw
        games = {}
        from yoolotto.lottery.enumerations import EnumerationManager
        games[game_type] = EnumerationManager.game(int(game_type))
        game = games[game_type]#LotteryGame Obj
        handler = game.handler
        component = handler.get_component1()
        try:
            if not component:
                component = game.components.all()[0]
        except:
            pass

        try:
            draw_record = LotteryDraw.objects.filter(date=draw.date,component_id=component)[0]
            results = json.loads(draw_record.result)
            play_data = json.loads(play.play)
            white = set(results[0:5]).intersection(set(play_data["numbers"][0:5]))
            megaball = results[5] == play_data["numbers"][5]
            if game_type == 0 or game_type == 13:

                if not white and not megaball:
                    return 0
                
                if len(white) == 0 and megaball:
                    return "MEGABALL_ONLY"
                    
                if len(white) == 1 and megaball:
                    return "ONE_OF_FIVE_MEGABALL"

                if len(white) == 2 and megaball:
                    return "TWO_OF_FIVE_MEGABALL"
                
                if len(white) == 3:
                    if megaball:
                        return "THREE_OF_FIVE_MEGABALL"
                    else:
                        return "THREE_OF_FIVE"

                if len(white) == 4:
                    if megaball:
                        return "FOUR_OF_FIVE_MEGABALL"
                    else:
                        return "FOUR_OF_FIVE"
                    
                if len(white) == 5:
                    if megaball:
                        return "JACKPOT"
                    else:
                        return "FIVE_OF_FIVE"
                
                return 0
                        
                raise RuntimeError("Unknown Win State?")

            else:

                if not white and not megaball:
                    return 0
                
                if len(white) == 0 and megaball:
                    return "POWERBALL_ONLY"
                    
                if len(white) == 1 and megaball:
                    return "ONE_OF_FIVE_POWERBALL"

                if len(white) == 2 and megaball:
                    return "TWO_OF_FIVE_POWERBALL"
                
                if len(white) == 3:
                    if megaball:
                        return "THREE_OF_FIVE_POWERBALL"
                    else:
                        return "THREE_OF_FIVE"

                if len(white) == 4:
                    if megaball:
                        return "FOUR_OF_FIVE_POWERBALL"
                    else:
                        return "FOUR_OF_FIVE"
                    
                if len(white) == 5:
                    if megaball:
                        return "JACKPOT"
                    else:
                        return "FIVE_OF_FIVE"
                
                return 0
                        
                raise RuntimeError("Unknown Win State?")
        except:
            pass

    def update_winnings(self,save=True):
        from yoolotto.lottery.game.manager import GameManager
        
        self.winnings = None
        if self.ticket.fantasy == False:    
            if self.ticket.draw.result:
                game = GameManager.get(self.ticket.draw.component.parent.code)
                _winnings = game.earnings(self)
                if not isinstance(_winnings, list):
                    winnings = _winnings
                else:
                    winnings = _winnings[0]
                
                if winnings == game.JACKPOT:
                    winnings = self.ticket.draw.jackpot

                try:                                                                                            
                    if game.NAME == 'FantasyFive':
                        winnings = \
                        self.calculate_fantasyfive_winnings(winnings, self.ticket.draw, game)
                    # check earnings for SuperlottoPlus
                    if game.NAME == 'SuperLottoPlus':
                        winnings = \
                        self.calculate_superlottoplus_winnings(winnings, self.ticket.draw, game)
                    # check earnings for Powerball
                    if game.NAME == "PowerballCA":
                        winnings = \
                        self.calculate_powerballca_winnings(winnings, self.ticket.draw, game)

                    if game.NAME == "Powerball":
                        winnings = \
                        self.calculate_powerball_winnings(winnings, self.ticket.draw, game)
                    # check earnings for Mega Millions
                    if game.NAME == "MegaMillions":
                        winnings = \
                        self.calculate_megamillion_winnings(winnings, self.ticket.draw, game)
                    if game.NAME == "MegaMillionsCA":
                        winnings = \
                        self.calculate_megamillionca_winnings(winnings, self.ticket.draw, game)
                        
                    # check earnings for DailyDerby
                    if game.NAME == "DailyDerby":
                        winnings = \
                        self.calculate_dailyderby_winnings(winnings, self.ticket.draw, game)
                    # check earnings for DailyFor
                    if game.NAME == "DailyFor":
                        
                        winnings = \
                        self.calculate_dailyfor_winnings(winnings, self.ticket.draw, game)
                        
                    # check earnings for DailyThree
                    if game.NAME == "DailyThree":
                        winnings = \
                        self.calculate_dailythree_winnings(winnings, self.ticket.draw, game)

                    # check earnings for DailyThree
                    if game.NAME == "AllOrNothing":
                        winnings = \
                        self.calculate_allornothing_winnings(winnings, self.ticket.draw, game)

                    if game.NAME == "Lotto":
                        winnings = \
                        self.calculate_lotto_winnings(winnings, self.ticket.draw, game)

                    if game.NAME == "TwoStep":
                        winnings = \
                        self.calculate_twostep_winnings(winnings, self.ticket.draw, game)
                except Exception as e:
                    print e
                    pass
                    
                self.winnings = winnings
                
                if not isinstance(_winnings, list):
                    self.winnings_base = winnings
                else:
                    try:
                        self.winnings_base = _winnings[1]
                    except:
                        pass
                    try:
                        self.winnings_sum = _winnings[2]
                    except:
                        self.winnings_sum = _winnings[1]
                    
            if save:
                self.save()
        else:
            print "in update winnings False"
            if self.ticket.draw.result:
                print self.ticket.ticket_submissions.all()
                game_type = self.ticket.ticket_submissions.all()[0].gameType

                _winnings = self.fantasy_earnings(self,game_type)
                print "_winnings",_winnings
                print "_winnings",_winnings
                if not isinstance(_winnings, list):
                    winnings = _winnings
                else:
                    winnings = _winnings[0]
                try:
                    if game_type == 0 or game_type == 13:
                        print "in gametype megamillion"
                        winnings_cal = self.calculate_fantasymegamillion_winnings(_winnings)
                    else:
                        winnings_cal = self.calculate_fantasypowerball_winnings(_winnings)
                #except:
                   # pass
                    print "winnings",winnings_cal
                    if "@" in winnings_cal:
                        winningss = winnings_cal.split('@')
                        self.winnings_coins = winningss[0]
                    elif "$" in winnings_cal:
                        winningss = winnings_cal.split('$')
                        self.winnings = winningss[0]
                    print "wingggsss afte cal",winnings
                except:
                    pass
            if save:
                print "in save"
                self.save()

            print "play",self.winnings

    class Meta:
        db_table = u"lottery_ticket_play"
        
class LotteryTicketAvailable(models.Model):
    """
    To determine wheather ticket result has been made available by data entry tool
    
    """    
    ticket = models.ForeignKey(LotteryTicket, related_name="ticket_submissions")
    play = models.ForeignKey(LotteryTicketPlay, related_name="ticket_play")
    available = models.BooleanField(default=False)
    json = models.TextField()
    device = models.CharField(max_length=256)
    image_first = models.ImageField(blank=True,upload_to="static/ticket/", null=True,max_length=300)
    image_second = models.ImageField(blank=True,upload_to="static/ticket/", null=True,max_length=300)
    image_third = models.ImageField(blank=True,upload_to="static/ticket/", null=True,max_length=300)
    valid_image_name = models.TextField(max_length=256,blank=True, null=True)
    rejected = models.BooleanField(default=False)
    reason = models.TextField(blank=True, null=True)
    defected = models.BooleanField(default=False)
    added_at = models.DateTimeField(auto_now_add=True)
    pending = models.BooleanField(default=True)
    gameType = models.CharField(max_length=100,null=True)

    class Meta:
        db_table = u"lottery_ticket_available"

class LotteryTicketEdit(models.Model):
    available = models.ForeignKey(LotteryTicketAvailable, related_name="ticket_available")
    numbers = models.CharField(max_length=256)

    class Meta:
        db_table = u"lottery_ticket_edit"    
        
#@receiver(post_save, sender=LotteryTicket)
#def update_ticket_from_self(sender, instance, signal, created, **kwargs):
#    instance.update()
class Fantasy_Help_Info(models.Model):
    question = models.TextField(blank=True, null=True)
    answer = models.TextField(blank=True, null=True)

    class Meta:
        db_table = u"lottery_help_info"


class Fantasy_Game_Info(models.Model):
    gameinfo = models.TextField()

    class Meta:
        db_table = u"lottery_game_info"