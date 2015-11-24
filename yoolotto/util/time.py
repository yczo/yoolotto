import datetime

TF_ISO_8601 = '%Y-%m-%dT%H:%M:%SZ'
TF_SHORT = '%Y-%m-%d'

def format_datetime(obj):
    if not obj:
        return None
    
    if isinstance(obj, datetime.datetime):
        return obj.strftime(TF_ISO_8601)
    elif isinstance(obj, datetime.date):
        return obj.strftime(TF_SHORT)
    else:
        raise NotImplementedError()