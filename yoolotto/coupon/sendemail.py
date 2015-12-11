#django imports
from django.template.context import Context
from django.template.loader import get_template
from django.core.mail.message import EmailMultiAlternatives

#yoolotto imports
from yoolotto.settings import DEFAULT_FROM_EMAIL
#-------------------------------------------------------------------------------

def common_send_email(subject, text_template_path, html_template_path,
                      context_data, recipients, from_email=None):
    """
    This method is a common method to send email via the bhane system.
    """
    coupon_obj = context_data['coupon_obj']
    if not from_email:
        from_email = DEFAULT_FROM_EMAIL

    #get templates from file system
    text_raw_content = get_template(text_template_path)                    
    try:
        html_raw_content = get_template(coupon_obj.vendor.email_content.path)#will return absolute path
    except:
    	html_raw_content = get_template(html_template_path)#else pickup common_vendor_email.html
    #render the raw data in the template
    d = Context(context_data)
    text_content = text_raw_content.render(d)
    html_content = html_raw_content.render(d)

    #contstruct the message and send it
    msg = EmailMultiAlternatives(subject, text_content, from_email, recipients)
    
    if coupon_obj.image.name:
        msg.attach_file(coupon_obj.image.path)
    msg.attach_alternative(html_content, "text/html")
    msg.send()
