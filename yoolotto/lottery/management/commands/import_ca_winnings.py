from django.core.management.base import BaseCommand, CommandError
from yoolotto.lottery.feed.data import LotteryWinningCADataProvider
from yoolotto.lottery.feed.winning_ca import LotteryWinningImporter

class Command(BaseCommand):
    help = 'Manual trigger for performing feed updates  '

    def handle(self, *args, **options):
        winning_ca = LotteryWinningImporter(provider=LotteryWinningCADataProvider.remote)
        winning_ca.run() 