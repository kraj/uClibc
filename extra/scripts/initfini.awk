#! /usr/bin/awk -f
# Contributed by Christian MICHON <christian_michon@yahoo.fr> to
#   eliminate the compile time dependancy on perl introduced by 
#   Erik's older initfini.pl 
# vim:ai:sw=2:

BEGIN \
{ alignval="";
  endp=0;
  end=0;
  system("/bin/touch crt[in].S");
  system("/bin/rm -f crt[in].S");
  omitcrti=0;
  omitcrtn=0;
  while(getline < "initfini.s")
  { if(/\.endp/) {endp=1}
    if(/\.end/) {end=1}
    if(/\.align/) {alignval=$2}
  }
  close("initfini.s");
}

/HEADER_ENDS/{omitcrti=1;omitcrtn=1;getline}
/PROLOG_BEGINS/{omitcrti=0;omitcrtn=0;getline}
/i_am_not_a_leaf/{getline}
/_init:/||/_fini:/{omitcrtn=1}
/PROLOG_PAUSES/{omitcrti=1;getline}
/PROLOG_UNPAUSES/{omitcrti=0;getline}
/PROLOG_ENDS/{omitcrti=1;getline}
/EPILOG_BEGINS/{omitcrtn=0;getline}
/EPILOG_ENDS/{omitcrtn=1;getline}
/TRAILER_BEGINS/{omitcrti=0;omitcrtn=0;getline}

/END_INIT/ \
{ if(endp)
  { gsub("END_INIT",".endp _init",$0)
  }
  else
  { if(end)
    { gsub("END_INIT",".end _init",$0)
    }
    else
    { gsub("END_INIT","",$0)
    }
  }
}

/END_FINI/ \
{ if(endp)
  { gsub("END_FINI",".endp _fini",$0)
  }
  else
  { if(end)
    { gsub("END_FINI",".end _fini",$0)
    }
    else
    { gsub("END_FINI","",$0)
    }
  }
}

/ALIGN/ \
{ if(alignval!="")
  { gsub("ALIGN",sprintf(".align %s",alignval),$0)
  }
  else
  { gsub("ALIGN","",$0)
  }
}

omitcrti==0 {print >> "crti.S"}
omitcrtn==0 {print >> "crtn.S"}

END \
{ close("crti.S");
  close("crtn.S");
}
