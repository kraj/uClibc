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
  glb_idx = 0;
  while(getline < "initfini.s")
  { if(/\.endp/) {endp=1}
    if(/\.end/) {end=1}
    if(/\.align/) {alignval=$2}
# here comes some special stuff for the SuperH targets
# Search for all labels
    if(/_GLOBAL_OFFSET_TABLE_/) {
      sub (":","",last);
      glb_label[glb_idx] = last;
      glb_idx += 1;
      glb = $0;
    }
    last = $1;
  }
  close("initfini.s");
}

/^_init:/{omitcrtn=1;if (glb_idx>0) print glb_label[0] ":" glb >> "crti.S";}
/^_fini:/{omitcrtn=1;if (glb_idx>1) print glb_label[1] ":" glb >> "crti.S";}
/HEADER_ENDS/{omitcrti=1;omitcrtn=1;getline}
/PROLOG_BEGINS/{omitcrti=0;omitcrtn=0;getline}
/i_am_not_a_leaf/{getline}
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
# substitude all label references of the _GLOBAL_OFFSET_TABLE_
# .L10  ==> .L10b etc.
glb_idx>0 { gsub (glb_label[0],sprintf("%sb",glb_label[0]),$0)}
glb_idx>1 { gsub (glb_label[1],sprintf("%sb",glb_label[1]),$0)}

omitcrti==0 {print >> "crti.S"}
omitcrtn==0 {print >> "crtn.S"}

END \
{ close("crti.S");
  close("crtn.S");
}
