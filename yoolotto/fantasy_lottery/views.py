
from __future__ import division
from django.shortcuts import render
from yoolotto.lottery.views import *
from yoolotto.lottery.ticket.views import *
from yoolotto.second_chance.models import *
from yoolotto.user.models import *
from yoolotto.coin.models import *
from yoolotto.lottery.models import LotteryDraw
from yoolotto.fantasy_lottery.models import *
from yoolotto.lottery.game.base import LotteryGame, LotteryPlayInvalidException, LotteryResultsInvalidException


# Create your views here.
def validate_numbers(numbers,game_type):
        if not numbers:
            raise LotteryPlayInvalidException("No number detected in white ball")
        
        if len(numbers)<5:
            raise LotteryPlayInvalidException("Please enter all white ball numbers")
        
        if len(numbers)==5:
            raise LotteryPlayInvalidException("Please enter Megaball number")
        print numbers
        print "numbers",type(numbers)
        white = numbers[0:5]
        powerball = numbers[5]
        print "white",white
        print "powerball",powerball
        print "length of white",len(set(white))
        if len(set(white)) != 5:
            raise LotteryPlayInvalidException("Duplicate Number %s" % white)
        if game_type == 21:#megamillion
            for number in white:
                if number not in xrange(1, 76):
                    raise LotteryPlayInvalidException("Please enter valid white ball number")
            
            if powerball not in xrange(1, 16):
                raise LotteryPlayInvalidException("Please enter valid Megaball number")    
              
        if game_type == 20:
            print "white",white
            for number in white:
                    if number not in xrange(1, 60): 
                        print "number",number
                        raise LotteryPlayInvalidException("Invalid White Ball Number %s" % number)

                    if powerball != -1:
                
                        if powerball not in xrange(1, 36):
                            raise LotteryPlayInvalidException("Invalid Powerball Number %s" % powerball) 
        return True 

class QuickPick(View):
    @rest
    @Authenticate()
    def post(self,request):
        data = json.loads(request.body)
        user = data["user"]
        user_id = user
        game_type = data["game_type"]
        division = data['division']
        submission = uuid.uuid4().hex[:16]
        print "submissssion",submission
        try:
            client_login_record, created = UserClientLogin.objects.get_or_create(device=request.yoo['device'])
        except:
            client_login_record = UserClientLogin.objects.filter(device=request.yoo['device'])[0]
        #date = datetime.datetime.strptime(draw_date, "%Y-%m-%d").date()
        lines_no = data['lines']
        date = datetime.datetime.strptime(data["drawingDate"], "%Y-%m-%d").date()
        if division == 'CA':
                game_types = {20:20,21:21}
                #components = {20:"Powerballca",21:"MegaMillionsCA"}
                division_id = 2
        else:
                game_types = {20:1,21:2}
                #components = {1:"Powerball",2:"MegaMillions"}
                division_id = 1
        game_types = game_types[game_type]
        draw, created = LotteryDraw.objects.get_or_create(component=game_types, date=date,division_id=division_id)
        ticket_record,created = FantasyTicket.objects.get_or_create(user_id = user_id,division = division,gameType = game_type,lines_no = lines_no,draw_id = draw.id)
        submission_record = None

        numbers = []
        if game_type == 21:#megamillion
            for line in range(0,lines_no):
                import random
                white_ball = random.sample(range(1,76),5)
                power_ball = random.sample(range(1,16),1)
                numbers1 = white_ball + power_ball
                #print numbers
                numbers.append(numbers1)
        else:
            for line in range(0,lines_no):
                import random
                white_ball = random.sample(range(1,60),5)
                power_ball = random.sample(range(1,36),1)
                numbers1 = white_ball + power_ball
                #print numbers
                numbers.append(numbers1)

        return {"numbers":numbers}

class SubmitNumbers(View):
    @rest
    @Authenticate()
    def post(self,request):
        data = json.loads(request.body)
        numbers = data['numbers']
        print "numbers in game",type(numbers)
        #user = data["user"]
        user = request.yoo["user"]
        user_id = user.id
        game_type = data["game_type"]
        division = data["division"]
        draw_date = data["drawingDate"]
        lines_no = data["lines"]
        coins = 2 * lines_no
        if division == 'CA':
                game_types = {1:20,0:21}
                components = {20:"Powerballca",21:"MegaMillionsCA"}
                division_id = 2
        else:
                game_types = {1:1,0:2}
                components = {1:"Powerball",2:"MegaMillions"}
                division_id = 1
        game_types = game_types[int(game_type)]
        submission_record = None
        submission = uuid.uuid4().hex[:16]
        date = datetime.datetime.strptime(draw_date, "%Y-%m-%d").date()
        component = LotteryGameComponent.objects.get(identifier=components[game_types])
        draw, created = LotteryDraw.objects.get_or_create(component=component, date=date,division_id=division_id)
        try:
            ticket_info,created = FantasyTicket.objects.get_or_create(user_id = user_id,division = division,gameType = game_type,lines_no = lines_no)
        except:
            ticket_info = FantasyTicket.objects.filter(user_id = user_id,division = division,gameType = game_type,lines_no = lines_no)#[0]


        updated_draw, created = LotteryDraw.objects.get_or_create(component=game_types,division_id=division_id,date=date)
        tickets = {}
        for i in numbers:
            validate_numbers(i,game_type)
        if updated_draw.id != ticket_info.draw_id :
                # create new ticket
            print updated_draw.id
            new_ticket_info, created = FantasyTicket.objects.get_or_create(user=ticket_info.user,draw=updated_draw,division=division)
            valid_draw = updated_draw.id
            valid_ticket = new_ticket_info.id
            tickets[valid_ticket] = new_ticket_info

            submission_record, created = FantasyTicketSubmission.objects.get_or_create(submission=submission, ticket_id=valid_ticket)
            submission_record.save()

            # create new play
            for line in numbers:
                serialized = dumps(line)
                play_record = FantasyTicketPlay(play=serialized,ticket_id=int(new_ticket_info.id),division = division)
                play_record.submission_old = submission
                play_record.submission = submission_record
                play_record.save()
        else:
                # - if draw is same
                valid_draw = ticket_info.draw_id
                valid_ticket = ticket_info.id
                tickets[valid_ticket] = ticket_info

                submission_record, created = FantasyTicketSubmission.objects.get_or_create(submission=submission, ticket_id=valid_ticket)
                submission_record.save()

                #update previous play
                
                for line in numbers:
                    serialized = dumps(line)
                    play_record, created = FantasyTicketPlay.objects.get_or_create(play=serialized,ticket=ticket_info,division=LotteryCountryDivision.objects.get(remote_id=division))
                    play_record.submission_old = submission
                    play_record.submission = submission_record
                    play_record.save()

        return {"success":True,"coins":coins}

class FantasyTicketCheck(View):
    @rest
    @Authenticate(create=False)
    def post(self, request, _id):
        user = request.yoo["user"]
        
        ticket = FantasyTicket.objects.get(pk=_id)
        
        if ticket.user_id!= user:
            raise exceptions.WebServiceAuthorizationFailed()
        
        if ticket.draw_id.result:
            for submission in ticket.submissions.all():
                submission.checked = True
                submission.save()
                    
        allocated = ticket.update(full=True)
        
        _result = ticket.representation()
        
        _result["coins"] = allocated
            
        return _result
    
    def put(self, *args, **kwargs):
        return self.post(*args, **kwargs)