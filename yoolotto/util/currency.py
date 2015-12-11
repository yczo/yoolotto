import decimal

def format(value):
    return "%.2f" % decimal.Decimal(value)

def decimalize(value):
    decimal.Decimal(format(value))