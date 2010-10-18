#!/bin/sh

estdir=/etc/pki/esteid
wwwdir=/var/www/esteid
crndir=/etc/cron.d

if [ `id -u` -gt 0 ]; then
    echo "ERROR: You must be root to run this script"
    exit 2
fi

mkdir -p "$estdir"/{ca,crl}

cd "$estdir"/ca || exit 2

echo "* Downloading CA certificates to $estdir/ca"
wget -nv -O "JUUR-SK.crt" http://www.sk.ee/files/JUUR-SK.PEM.cer
wget -nv -O "ESTEID-SK.crt" http://www.sk.ee/files/ESTEID-SK.PEM.cer
wget -nv -O "ESTEID-SK 2007.crt" http://www.sk.ee/files/ESTEID-SK%202007.PEM.cer
for f in *.crt;do ln -sf "$f" `openssl x509 -hash -noout -in "$f"`.0; done

if [ ! -f "$wwwdir"/index.php ]; then
    echo "* Creating SSL test page to $wwwdir"
    mkdir -p "$wwwdir"
    echo '<pre> <?php print_r($_SERVER); ?> </pre>' > "$wwwdir"/index.php
fi

conf=/root/esteid.conf

# RedHat
[ -d "/etc/httpd/conf.d" ] && \
    conf=/etc/httpd/conf.d/esteid.conf

# Debian / Ubuntu
[ -d "/etc/apache2/sites-available" ] && \
    conf=/etc/apache2/sites-available/esteid

if [ ! -f "$conf" ]; then
    echo "* Creating apache configuration template $conf."
    cat > "$conf" <<E_O_F

# Generated according to instructions at
# http://code.google.com/p/esteid/wiki/AuthConfApache

<VirtualHost MY-IP-ADDRESS:443>
    SSLEngine on

    # Set document root and site name
    DocumentRoot "/var/www/esteid"
    ServerName MY-SITE-NAME

    # Where to find server SSL certificate and key
    SSLCertificateFile /etc/pki/tls/certs/MY-SITE-NAME.crt
    SSLCertificateKeyFile /etc/pki/tls/private/MY-SITE-NAME.key

    # Where to find ID-card CA certificates and CRLs
    SSLCACertificatePath /etc/pki/esteid/ca
    SSLCARevocationPath /etc/pki/esteid/crl

    # Require Authentication with ID-card
    SSLVerifyClient require
    SSLVerifyDepth  2

    # Make SSL session data available to scripts
    <Files ~ "\.(cgi|shtml|php)$">
        SSLOptions +StdEnvVars +ExportCertData
    </Files>
</VirtualHost>
E_O_F
fi

echo "* Creating CRL update script"
cat > "$estdir"/update-crl.sh <<E_O_F
#!/bin/sh

cd "$estdir"/crl || exit 2

echo -n "Starting CRL download: "
LANG=C date
wget -nv -O "esteid.der" http://www.sk.ee/crls/esteid/esteid.crl
wget -nv -O "esteid2007.der" http://www.sk.ee/crls/esteid/esteid2007.crl   
wget -nv -O "crl.der" http://www.sk.ee/crls/juur/crl.crl 

echo "Converting CRLs to PEM format and generating hash links"
rm -f *.r0
for f in *.der; do
    p=\`basename "\$f" .der\`.crl
    openssl crl -in "\$f" -out "\$p" -inform DER
    ln -s "\$p" \`openssl crl -hash -noout -in "\$p"\`.r0
    rm -f "\$f"
done

echo "Notifying Apache about new CRL-s"
# This MUST be a full restart, otherwise CRL-s will not be reloaded
[ -x /etc/init.d/httpd ] && /etc/init.d/httpd restart
[ -x /etc/init.d/apache2 ] && /etc/init.d/apache2 restart
E_O_F
chmod a+x "$estdir"/update-crl.sh

if [ ! -f "$crndir"/esteid ]; then
    echo "* Adding CRL updater to crontab"
    cat >> "$crndir"/esteid <<E_O_F
5 6,18 * * * root $estdir/update-crl.sh > /var/log/esteid-update.log 2>&1
E_O_F
fi

cat <<E_O_F
==========================================================================
|
| Your server has been almost set up to use ID-Card for authentication.
|
| You Must edit Apache configuration file $conf
| to set up server name, IP-address and SSL Certificate
| 
| Your server has been configured to donload CRL-s once every twelve hours
| These files are quite large (10Mb at least) so it's better to disable
| CRL updates if You don't need them.
| Edit $crndir/esteid to disable automatic CRL updates
|
==========================================================================
E_O_F
