macro PUSH [reg] {
   forward
      PUSH reg
}

macro POP [reg] {
   reverse
      POP reg
}

struc ds [val] {
      common  
      . db val
      .size = $ - .
}