# Create your models here.
import json
from decimal import Decimal
import datetime
from django.db import models
from django.db import connection
from django.db.models.signals import post_save
from yoolotto.lottery.models import * 

class FantasyTicket(models.Model):
    draw = models.ForeignKey(LotteryDraw, related_name="fantasy_tickets_draw")
    user = models.ForeignKey("user.YooLottoUser",related_name="fantasy_tickets_user")
    division = models.CharField(max_length=18)
    winnings = models.DecimalField(max_digits=15, decimal_places=2, blank=True, null=True)
    checked = models.BooleanField(default=False)
    notified = models.BooleanField(default=False)
    deleted = models.BooleanField(default=False)    
    added_at = models.DateTimeField(auto_now_add=True)
    available = models.BooleanField(default=False)
    gameType = models.IntegerField(default=50)
    lines_no = models.IntegerField(default=0)
    #lottery_ticket = models.ForeignKey(LotteryTicket,related_name = "fantasy_tickets")

    def representation(self, 
        representation=representations.TicketRepresentationShortList,
        *args, **kwargs):
        return representation(self)(*args, **kwargs)
    
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
        self.update_winnings(save=False)
        
        self.save()
                
        return allocated
        
    def update_coins(self, device, Client_login, save=False):
        manager = CoinTicketManager(device, Client_login, ticket=self)
        manager.update()
        
        if save:
            self.save()
        
        return 10
    
    def update_winnings(self, save=True):
        if not self.draw.result:
            self.winnings = None
            if save:
                save.save()
            
            return
        
        total = Decimal("0.00")
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
    def all_checked(self):
        return False if sum([True if not x["checked"] else False for x in self.submissions.all().values("checked")]) > 0 else True

    class Meta:
        db_table = u"fantasy_ticket"

class FantasyTicketClient(models.Model):
    draw = models.ForeignKey(LotteryDraw, related_name="fantasy_client")
    device = models.CharField(max_length=255, blank=True, null=True)
    email = models.CharField(max_length=255, blank=True, null=True)
    added_at = models.DateTimeField(auto_now_add=True)

    class Meta:
        db_table = u"fantasy_ticket_client"

        
        
class FantasyTicketSubmission(models.Model):
  
    submission = models.CharField(max_length=16, blank=True, null=True)
    ticket = models.ForeignKey(FantasyTicket, related_name="fantasy_submissions")
    checked = models.BooleanField(default=False)
    added_at = models.DateTimeField(auto_now_add=True)
    
    class Meta:
        db_table = u"fantasy_ticket_submission"

class FantasyTicketPlay(models.Model):
    ticket = models.ForeignKey(FantasyTicket, related_name="fantasy_plays")
    play = models.CharField(max_length=255, blank=True, null=True)
    division = division = models.CharField(max_length=18)
    submission_old = models.CharField(max_length=16, blank=True, null=True, db_column="fantasy_submission")
    submission = models.ForeignKey(FantasyTicketSubmission, related_name="fantasy_plays", blank=True, null=True)
    
    winnings = models.DecimalField(max_digits=15, decimal_places=2, blank=True, null=True)
    
    added_at = models.DateTimeField(auto_now_add=True)
    
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

    def calculate_powerball_winnings(self, winnings, draw, game):
        """
        This will return the winning amount as per request
        """
        component = LotteryGameComponent.objects.get(parent__code=game.NAME)
        
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


    
    
    def update_winnings(self, save=True):
        from yoolotto.lottery.game.manager import GameManager
        
        self.winnings = None
        
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

                if game.NAME == "Powerball":
                    winnings = \
                    self.calculate_powerball_winnings(winnings, self.ticket.draw, game)
                # check earnings for Mega Millions
                if game.NAME == "MegaMillions":
                    winnings = \
                    self.calculate_megamillion_winnings(winnings, self.ticket.draw, game)
               
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
            
    class Meta:
        db_table = u"fantasy_ticket_play"
        
