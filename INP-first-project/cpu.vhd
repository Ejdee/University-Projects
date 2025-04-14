-- cpu.vhd: Simple 8-bit CPU (BrainFuck interpreter)
-- Copyright (C) 2024 Brno University of Technology,
--                    Faculty of Information Technology
-- Author(s): Adam Behoun <xbehoua00@stud.fit.vutbr.cz>
--
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

-- ----------------------------------------------------------------------------
--                        Entity declaration
-- ----------------------------------------------------------------------------
entity cpu is
 port (
   CLK   : in std_logic;  -- hodinovy signal
   RESET : in std_logic;  -- asynchronni reset procesoru
   EN    : in std_logic;  -- povoleni cinnosti procesoru
 
   -- synchronni pamet RAM
   DATA_ADDR  : out std_logic_vector(12 downto 0); -- adresa do pameti
   DATA_WDATA : out std_logic_vector(7 downto 0); -- mem[DATA_ADDR] <- DATA_WDATA pokud DATA_EN='1'
   DATA_RDATA : in std_logic_vector(7 downto 0);  -- DATA_RDATA <- ram[DATA_ADDR] pokud DATA_EN='1'
   DATA_RDWR  : out std_logic;                    -- cteni (1) / zapis (0)
   DATA_EN    : out std_logic;                    -- povoleni cinnosti
   
   -- vstupni port
   IN_DATA   : in std_logic_vector(7 downto 0);   -- IN_DATA <- stav klavesnice pokud IN_VLD='1' a IN_REQ='1'
   IN_VLD    : in std_logic;                      -- data platna
   IN_REQ    : out std_logic;                     -- pozadavek na vstup data
   
   -- vystupni port
   OUT_DATA : out  std_logic_vector(7 downto 0);  -- zapisovana data
   OUT_BUSY : in std_logic;                       -- LCD je zaneprazdnen (1), nelze zapisovat
   OUT_INV  : out std_logic;                      -- pozadavek na aktivaci inverzniho zobrazeni (1)
   OUT_WE   : out std_logic;                      -- LCD <- OUT_DATA pokud OUT_WE='1' a OUT_BUSY='0'

   -- stavove signaly
   READY    : out std_logic;                      -- hodnota 1 znamena, ze byl procesor inicializovan a zacina vykonavat program
   DONE     : out std_logic                       -- hodnota 1 znamena, ze procesor ukoncil vykonavani programu (narazil na instrukci halt)
 );
end cpu;

-- ----------------------------------------------------------------------------
--                      Architecture declaration
-- ----------------------------------------------------------------------------

architecture behavioral of cpu is

    -- SIGNALS BASED ON THE FSM
    signal pc_reg : std_logic_vector(12 downto 0);
    signal pc_inc : std_logic;
    signal pc_dec : std_logic;
    signal tmp_reg : std_logic_vector(7 downto 0);
    signal tmp_ld : std_logic;
    signal cnt_load : std_logic;
    signal cnt_reg : std_logic_vector(7 downto 0);
    signal cnt_inc : std_logic;
    signal cnt_dec : std_logic;
    signal cnt_zero : std_logic;
    signal ptr_reg : std_logic_vector(12 downto 0);
    signal ptr_rst : std_logic;
    signal ptr_inc : std_logic;
    signal ptr_dec : std_logic;

    -- SIGNALS FOR MUX2TO1
    signal sel2to1 : std_logic;
    
    -- SIGNALS FOR MUX4TO1
    signal sel4to1 : std_logic_vector(1 downto 0);

    -- FSM STATES
    type state_type is (
        init_start, init_start_find_x,
        idle, fetch, decode,
        inc_ptr,
        dec_ptr,
        inc_curr, inc_curr_ns, 
        dec_curr, dec_curr_ns, 
        loop_s, loop_s_check, loop_s_search, loop_s_compare, loop_s_check_cnt_zero, loop_s_wait_for_cnt_zero_change,
        loop_e, loop_e_check, loop_e_search, loop_e_prepare_search, loop_e_check_cnt_zero, loop_e_wait_for_cnt_zero_change,
        load_tmp, load_tmp_ns, 
        load_tmp_to_curr, load_tmp_to_curr_ns,
        print_curr, print_curr_ns, 
        fetch_load_curr, fetch_load_curr_ns, 
        continue_read, 
        halt
    );

    signal pstate : state_type;
    signal nstate : state_type;
    

begin

 -- pri tvorbe kodu reflektujte rady ze cviceni INP, zejmena mejte na pameti, ze 
 --   - nelze z vice procesu ovladat stejny signal,
 --   - je vhodne mit jeden proces pro popis jedne hardwarove komponenty, protoze pak
 --      - u synchronnich komponent obsahuje sensitivity list pouze CLK a RESET a 
 --      - u kombinacnich komponent obsahuje sensitivity list vsechny ctene signaly. 

    -- Program counter PC
    pc_cntr: process (RESET, CLK)
    begin
        if (RESET='1') then
            pc_reg <= (others=>'0');
        elsif (CLK'event) and (CLK='1') then
            if (pc_inc='1') then
                pc_reg <= pc_reg + 1;
            elsif (pc_dec='1') then
                pc_reg <= pc_reg - 1;
            end if;
        end if;
    end process;

    -- Temporary register
    tmp_proc : process (RESET, CLK)
    begin
        if (RESET='1') then
            tmp_reg <= (others=>'0');
        elsif (CLK'event) and (CLK='1') then
            if (tmp_ld='1') then
                tmp_reg <= DATA_RDATA;
            end if;
        end if;
    end process;

    -- Counter register
    cnt_proc : process (RESET, CLK)
    begin
        if (RESET='1') then
            cnt_reg <= (others=>'0');
            cnt_zero <= '0';
        elsif (CLK'event) and (CLK='1') then
            if (cnt_inc='1') then 
                cnt_reg <= cnt_reg+1;
            elsif (cnt_dec='1') then
                cnt_reg <= cnt_reg-1;
            elsif (cnt_load='1') then
                cnt_reg <= X"01";
            end if;

            if cnt_reg="00000000" then
                cnt_zero <= '1';
            else
                cnt_zero <= '0';
            end if;
        end if;
    end process;

    -- Pointer register
    ptr_proc : process (RESET, CLK)
    begin
        if (RESET='1') then
            ptr_reg <= (others=>'0');
        elsif (CLK'event) and (CLK='1') then
            if (ptr_rst='1') then
                ptr_reg <= (others=>'0'); 
            elsif (ptr_dec='1') then
                if(ptr_reg="0000000000000") then
                    ptr_reg <= "1111111111111";
                else
                    ptr_reg <= ptr_reg-1;
                end if;
            elsif (ptr_inc='1') then
                if(ptr_reg="1111111111111") then
                    ptr_reg <= "0000000000000";
                else
                    ptr_reg <= ptr_reg+1;
                end if;
            end if;
        end if;
    end process;

    -- MUX 2 TO 1
    mux2to1 : process(ptr_reg, pc_reg, sel2to1)
    begin
        case sel2to1 is
            when '0' =>
                DATA_ADDR <= ptr_reg;
            when '1' =>
                DATA_ADDR <= pc_reg;
            when others =>
                DATA_ADDR <= (others => '0');
        end case;
    end process;

    -- MUX 4 to 1
    mux4to1: process(sel4to1, IN_DATA, tmp_reg, DATA_RDATA)
    begin
        case sel4to1 is
            when "00" =>
                DATA_WDATA <= IN_DATA;
            when "01" =>
                DATA_WDATA <= tmp_reg;
            when "10" =>
                DATA_WDATA <= DATA_RDATA - 1;
            when "11" =>
                DATA_WDATA <= DATA_RDATA + 1;
            when others =>
                DATA_WDATA <= (others => '0');
        end case;
    end process;

    fsm_pstate : process (RESET, CLK)
    begin
        if (RESET='1') then
            pstate <= init_start;
        elsif (CLK'event) and (CLK='1') then
            if (EN='1') then
                pstate <= nstate;
            end if;
        end if;
    end process;

    fsm_next_state : process (pstate, cnt_zero, DATA_RDATA, OUT_BUSY, IN_VLD) 
    begin
        DATA_EN <= '0';
        DATA_RDWR <= '1'; -- reading
        IN_REQ <= '0';
        OUT_WE <= '0';
        OUT_INV <= '0';
        OUT_DATA <= (others=>'0');
        pc_inc <= '0';
        pc_dec <= '0';
        tmp_ld <= '0';
        cnt_load <= '0';
        cnt_inc <= '0';
        cnt_dec <= '0';
        ptr_rst <= '0';
        ptr_inc <= '0';
        ptr_dec <= '0';
        sel2to1 <= '1';
        sel4to1 <= "00";
        case pstate is
            when init_start =>
                READY <= '0';
                DONE <= '0';
                nstate <= init_start_find_x;

            -- Stay in this state until the mem[PTR] is not '@'
            when init_start_find_x =>
                DATA_EN <= '1';
                DATA_RDWR <= '1';
                sel2to1 <= '0';

                if DATA_RDATA=X"40" then
                    READY <= '1';
                    nstate <= fetch;
                else 
                    ptr_inc <= '1';
                    nstate <= init_start_find_x;
                end if;

            -- Load mem[PC] to RDATA
            when fetch =>
                DATA_EN <= '1';
                DATA_RDWR <= '1';
                sel2to1 <= '1';
                nstate <= decode;
            
            when decode =>
                sel2to1 <= '1';
                case DATA_RDATA is
                    when X"3E" =>
                        nstate <= inc_ptr;
                    when X"3C" =>
                        nstate <= dec_ptr;
                    when X"2B" =>
                        nstate <= inc_curr;
                    when X"2D" => 
                        nstate <= dec_curr;
                    when X"5B" =>
                        nstate <= loop_s;
                    when X"5D" =>
                        nstate <= loop_e;
                    when X"24" =>
                        nstate <= load_tmp;
                    when X"21" =>
                        nstate <= load_tmp_to_curr;
                    when X"2E" =>
                        nstate <= print_curr;
                    when X"2C" =>
                        nstate <= fetch_load_curr;
                    when X"40" =>
                        nstate <= halt;
                    when others =>
                        nstate <= continue_read;
                end case;
            when halt =>
                DONE <= '1';
                nstate <= halt;

            -- Increment the program counter and get back to fetch state
            when inc_ptr =>
                pc_inc <= '1';
                ptr_inc <= '1';
                nstate <= fetch;

            -- Decrement the program counter and get back to fetch state
            when dec_ptr =>
                pc_inc <= '1';
                ptr_dec <= '1';
                nstate <= fetch;

            -- load mem[PTR] to RDATA
            when inc_curr =>
                sel2to1 <= '0';
                DATA_EN <= '1';
                DATA_RDWR <= '1';
                nstate <= inc_curr_ns;
                
            -- After it is loaded, we can write RDATA + 1 to mem[PTR] and increment the program counter
            when inc_curr_ns =>
                sel2to1 <= '0';
                sel4to1 <= "11";
                DATA_EN <= '1';
                DATA_RDWR <= '0'; -- writing
                pc_inc <= '1';
                nstate <= fetch;

            -- load mem[PTR] to RDATA
            when dec_curr =>
                sel2to1 <= '0';
                DATA_EN <= '1';
                DATA_RDWR <= '1';
                nstate <= dec_curr_ns;

            -- After it is loaded, we can write RDATA - 1 to mem[PTR] and increment the program counter
            when dec_curr_ns =>
                sel2to1 <= '0';
                sel4to1 <= "10";
                DATA_EN <= '1';
                DATA_RDWR <= '0';
                pc_inc <= '1';
                nstate <= fetch;

            -- First we need to update DATA_RDATA, only then we can write those data to OUT_DATA
            -- So unlike the pseudocode, we doing OUT DATA <- mem[PTR] and then while(OUT_BUSY) {}, PC <- PC + 1
            when print_curr =>
                sel2to1 <= '0';
                DATA_EN <= '1';
                DATA_RDWR <= '1';
                nstate <= print_curr_ns;

            -- If the OUT_BUSY is not '1', we load the RDATA to OUT_DATA, set the OUT_WE signal to '1', increment the program counter
            -- and go to fetch state
            when print_curr_ns =>
                if (OUT_BUSY='1') then
                    nstate <= print_curr;
                else
                    OUT_DATA <= DATA_RDATA;
                    OUT_WE <= '1';
                    pc_inc <= '1';
                    nstate <= fetch;
                end if;

            -- Set the IN_REQ to '1' and stay in this state until IN_VLD is not '1'
            when fetch_load_curr =>
                IN_REQ <= '1';
                if (IN_VLD='0') then
                    nstate <= fetch_load_curr;
                else
                    nstate <= fetch_load_curr_ns;
                end if;
            
            -- Load the IN_DATA to mem[PTR] and increment the program counter
            when fetch_load_curr_ns =>
                sel2to1 <= '0';
                DATA_EN <= '1';
                DATA_RDWR <= '0';
                sel4to1 <= "00";
                pc_inc <= '1';
                nstate <= fetch;

            -- Load mem[PTR] to RDATA
            when load_tmp =>
                sel2to1 <= '0';
                DATA_EN <= '1';
                DATA_RDWR <= '1';
                nstate <= load_tmp_ns;

            -- Load RDATA to TMP register and increment the program counter
            when load_tmp_ns =>
                tmp_ld <= '1';
                pc_inc <= '1';
                nstate <= fetch;

            -- Load mem[PTR] to the RDATA
            when load_tmp_to_curr =>
                sel2to1 <= '0';
                DATA_EN <= '1';
                DATA_RDWR <= '1'; -- reading
                nstate <= load_tmp_to_curr_ns;
                
            -- Write the value in TMP register to mem[PTR] and increment the program counter
            when load_tmp_to_curr_ns =>
                sel2to1 <= '0';
                sel4to1 <= "01";
                DATA_EN <= '1';
                DATA_RDWR <= '0'; -- writing
                pc_inc <= '1';
                nstate <= fetch;

            -- Start of loop, where we increment the program counter and load mem[PTR] to RDATA
            when loop_s =>
                pc_inc <= '1';
                sel2to1 <= '0';
                DATA_EN <= '1';
                DATA_RDWR <= '1';
                nstate <= loop_s_check;

            -- In next state, we can compare the loaded RDATA since we are in next clock cycle and 
            -- if the RDATA is 0, we load 1 in counter register and go in next state, otherwise we go to fetch state
            when loop_s_check =>
                if DATA_RDATA=X"00" then
                    cnt_load <= '1';
                    nstate <= loop_s_search;
                else 
                    nstate <= fetch;
                end if;

            -- In next loop state, we load mem[PC] in RDATA to be able to compare with it in next clock cycle
            when loop_s_search =>
                sel2to1 <= '1';
                DATA_EN <= '1';
                DATA_RDWR <= '1';
                nstate <= loop_s_compare;

            -- If the loaded RDATA is '[', increment the counter register, if the loaded RDATA is ']', we decrement it
            when loop_s_compare =>
                if DATA_RDATA=X"5B" then
                    cnt_inc <= '1';
                elsif DATA_RDATA=X"5D" then
                    cnt_dec <= '1';
                end if;

                nstate <= loop_s_wait_for_cnt_zero_change;

            -- We have to wait for cnt_zero signal to load, since we haven't implemented it like another process.
            -- That will take full one clock cycle - not optimal, but there is no optimalization required in this project
            when loop_s_wait_for_cnt_zero_change =>
                nstate <= loop_s_check_cnt_zero;

            -- In this state, we increment the program counter and if the counter register is 0 - that means the signal cnt_zero is '1',
            -- we go to the fetch state. Otherwise loop back to the loop_s_search state
            when loop_s_check_cnt_zero =>
                pc_inc <= '1';
                if cnt_zero='1' then
                    nstate <= fetch;
                else
                    nstate <= loop_s_search;
                end if;

            -- End of the loop. We load mem[PTR] in the RDATA
            when loop_e =>
                sel2to1 <= '0';
                DATA_EN <= '1';
                DATA_RDWR <= '1';
                nstate <= loop_e_check;

            -- Since it is loaded now, we can compare it with 0, in that case we increment the program counter and go to fetch state
            -- Otherwise we load 1 in counter register and decrement the program counter
            when loop_e_check =>
                if DATA_RDATA=X"00" then
                    pc_inc <= '1';
                    nstate <= fetch;
                else
                    cnt_load <= '1';
                    pc_dec <= '1';
                    nstate <= loop_e_prepare_search;
                end if;

            -- Load mem[PC] in the RDATA
            when loop_e_prepare_search =>
                sel2to1 <= '1';
                DATA_EN <= '1';
                DATA_RDWR <= '1';
                nstate <= loop_e_search;
                
            -- Compare the loaded RDATA with '[' = decrement counter register or ']' = increment counter register
            when loop_e_search =>
                if DATA_RDATA=X"5D" then
                    cnt_inc <= '1';
                elsif DATA_RDATA=X"5B" then
                    cnt_dec <= '1';
                end if;

                nstate <= loop_e_wait_for_cnt_zero_change;

            -- We have to wait for cnt_zero signal to change, since it will take one full cycle as in loop_start
            when loop_e_wait_for_cnt_zero_change =>
                nstate <= loop_e_check_cnt_zero;
            
            -- If the counter register is all 0's, we increment the program counter and go to fetch state
            -- otherwise we decrement the program counter and loop back to looping state
            when loop_e_check_cnt_zero =>
                if cnt_zero='1' then
                    pc_inc <= '1';
                    nstate <= fetch;
                else
                    pc_dec <= '1';
                    nstate <= loop_e_prepare_search;
                end if;

            -- Since we haven't decoded any instruction symbol, we want to increment the program counter and go to fetch state again
            when continue_read =>
                pc_inc <= '1';
                nstate <= fetch;

            when others =>
                    nstate <= idle;
        end case;
    end process;



end behavioral;
