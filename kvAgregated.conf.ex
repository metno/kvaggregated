
#konfigurasjons file for � kj�re kvalobs lokalt p� glacier

corba{
  #Where is CORBA nameserver
  nameserver="corbans.oslo.dnmi.no"

  #Which path in CORBA nameserver shall we use for
  #this instance of kvalobs
  #path="kvspice"
  path="kvtest"
}

database{
  #Which database driver shall be used. 
  dbdriver="pgdriver.so"

  #The string to be used to connect to the database.
  #ex. for a postgresql databse
  #dbconnect="user=kvalobs dbname=kvalobs password= host= port="
  dbconnect="user=kvalobs dbname=kvalobs password=kvalobs12 host=localhost"
}

kvManagerd{
}

kvDataInputd{
}


kvQabased{
}
