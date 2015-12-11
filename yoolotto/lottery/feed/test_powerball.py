from yoolotto.lottery.models import LotteryDraw
from datetime import datetime,date
import ast

for d in LotteryDraw.objects.filter(component__name="powerball",date__gt=date(2014,4,1)):
    if d.component.name=="Powerball" and d.result and len(ast.literal_eval(d.result))==7:
        print d.result,d.id
        d.result=str(ast.literal_eval(d.result)[0:6])
        d.save()
print "<<DONE>>"
        
        