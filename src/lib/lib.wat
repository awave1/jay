(module
  (import "host" "exit" (func $exit))
  (import "host" "getchar" (func $getchar (result i32)))
  (import "host" "putchar" (func $putchar (param i32)))

  ;;
  ;; void halt();
  ;; 
  ;; @brief Function that stops execution
  ;;
  (func $halt (export "halt")
    call $exit
  )

  ;;
  ;; void printc(int char);
  ;; 
  ;; @brief Function takes in a single character and prints it to stdout
  ;; @param $char: i32 an integer representation of a single character
  ;;
  (func $printc (export "printc") (param $char i32)
    local.get $char
    call $putchar
  )

  ;;
  ;; void prints(string str);
  ;; 
  ;; @brief Function takes in a string and prints it to stdout
  ;; @param $str: i32 position of the string to print
  ;;
  (func $prints (export "prints") (param $str i32)
    local.get $str
    call $printc
  )

  ;;
  ;; void printb(boolean bool);
  ;; 
  ;; @brief Function takes in a boolean and prints it to stdout
  ;; @param $str: i32 bool value, 0 for false 1 for true
  ;;
  (func $printb (export "printb") (param $bool i32)
    local.get $bool
    ;; TODO: Call prints
    call $printc
  )

  ;;
  ;; @brief Helper function that gets ASCII representation of a single digit
  ;; @param $num: i32 integer to convert
  ;; @return i32 ASCII value of a $num
  ;;
  (func $_get_int_char (param $num i32) (result i32)
    local.get $num
    i32.const 48  ;; '0' in ASCII
    i32.add       ;; add value of '0' to integer that needs to be printed
  )

  ;;
  ;; @brief Helper function that gets the power of passed integer.
  ;;        For example, an integer 1234 will return power 3 (10^3 = 1000)
  ;; @return i32 power of the $num
  ;;
  (func $_get_exponent (param $num i32) (result i32)
    (local $power i32)

    loop $pow
      local.get $num
      i32.const 10
      i32.div_u
      local.set $num
      
      ;; $power += 1
      i32.const 1
      local.get $power
      i32.add
      local.set $power

      ;; if $num != 0, keep looping
      local.get $num
      i32.const 0
      i32.ne
      br_if $pow
    end $pow

    local.get $power
    i32.const 1
    i32.sub
    local.set $power

    local.get $power
  )

  ;;
  ;; @brief Raise to to the given power and return the result.
  ;;        For example, 10^2 == 100, 10^0 == 1
  ;; @param $exp: i32, exponent
  ;; @return i32, 10 to the power of $exp
  ;;
  (func $_10_to_pow (param $exp i32) (result i32)
    (local $result i32)
    (local $base i32)

    i32.const 1
    local.set $result

    i32.const 10
    local.set $base

    block $_outer
      local.get $exp
      i32.const 0
      i32.eq
      br_if $_outer

      local.get $exp
      i32.const 0
      i32.lt_s
      br_if $_outer

      loop $_exp
        ;; $result = $result * $base
        local.get $result
        local.get $base
        i32.mul
        local.set $result

        ;; exp -= 1
        local.get $exp
        i32.const 1
        i32.sub
        local.set $exp

        local.get $exp
        i32.const 0
        i32.ne
        br_if $_exp
      end $_exp
    end $_outer


    local.get $result
  )

  ;;
  ;; void printi(int num);
  ;; 
  ;; @brief Function that prints an integer.
  ;; @param $num: i32, an integer to print
  ;;
  (func $printi (export "printi") (param $num i32)
    (local $last_int i32)
    (local $exponent i32)
    (local $decimal i32)
    (local $num_part i32)
    
    block $_if_neg
      ;; if $num >= 0, branch out
      local.get $num
      i32.const 0
      i32.ge_s
      br_if $_if_neg
      
      ;; otherwise print minus sign
      i32.const 45  ;; '-' in ASCII
      call $printc

      ;; and negate the number before printing digits
      i32.const 0   ;; negate $num by doing 0 - $num
      local.get $num
      i32.sub
      local.set $num
    end $_if_neg
    
    ;; find out the exponent of the given number
    local.get $num
    call $_get_exponent
    local.set $exponent

    ;; get the 10^$exponent
    local.get $exponent
    call $_10_to_pow
    local.set $decimal

    ;; iterate to print number by number
    block $_outer
      loop $print_num
        ;; calculate $num / 10^$exponent
        local.get $num
        local.get $decimal
        i32.div_u
        local.set $num_part ;; save it to $num_part

        ;; get the leftmost digit to print
        ;; $num_part % 10
        local.get $num_part
        i32.const 10
        i32.rem_u
        call $_get_int_char
        call $printc

        ;; go to the next exponent
        ;; $exponent -= 1
        local.get $exponent
        i32.const 1
        i32.sub
        local.set $exponent

        ;; if $exponent less than 0, branch out
        local.get $exponent
        i32.const 0
        i32.lt_s
        br_if $_outer

        ;; otherwise update the $decimal
        local.get $exponent
        call $_10_to_pow
        local.set $decimal

        ;; keep iterating if $exponent >= 0
        local.get $exponent
        i32.const 0
        i32.ge_u
        br_if $print_num
      end $print_num
    end $_outer
  )


  ;; test main function
  (func $main
    i32.const -123
    call $printi
    
    i32.const 10 
    call $printc

    i32.const 123
    call $printi

    i32.const 10 
    call $printc
  )

  (start $main)
)
