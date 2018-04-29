library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.STD_LOGIC_ARITH.ALL;
use IEEE.STD_LOGIC_UNSIGNED.ALL;

entity debounce_inv is
  port(
		clk, reset_n  : in std_logic;
		button_n : in std_logic;
		pressed  : out std_logic
	  );
end debounce_inv;

architecture behavioral of debounce_inv is

	type state_type is (steady_zero, steady_one, potential_zero, potential_one);

	signal state : state_type;
	
	constant BOUNCING_LIMIT : natural := 100;
	signal debounce_counter : natural range 0 to BOUNCING_LIMIT;
	
begin

	debounce_fsm: process (clk, reset_n, button_n)
	begin
		if reset_n = '0' then
		
			debounce_counter <= 0;
			
			if button_n = '1' then
				state <= steady_zero;
				pressed <= '0';
		   else
				state <= steady_one;
				pressed <= '1';
			end if;
			
		elsif (rising_edge(clk)) then
			case state is
			
				when steady_zero =>
					
					pressed <= '0';
					
				   debounce_counter <= 0;
					
					if button_n = '0' then
						state <= potential_one;
					else
						state <= steady_zero;
					end if;
					
				when potential_one =>
					
					pressed <= '0';
					
					if button_n = '1' then
						state <= steady_zero;
					elsif debounce_counter >= BOUNCING_LIMIT then
						state <= steady_one;
					else
						state <= potential_one;
						debounce_counter <= debounce_counter + 1;
					end if;
					
				when steady_one =>
					
					pressed <= '1';
					
				   debounce_counter <= 0;
					
					if button_n = '1' then
						state <= potential_zero;
					else
						state <= steady_one;
					end if;
					
				when potential_zero =>
					
					pressed <= '1';
					
					if button_n = '0' then
						state <= steady_one;
					elsif debounce_counter >= BOUNCING_LIMIT then
						state <= steady_zero;
					else
						state <= potential_zero;
						debounce_counter <= debounce_counter + 1;
					end if;
					
				when others =>
				   debounce_counter <= 0;
					if button_n = '1' then
						state <= steady_zero;
						pressed <= '0';
					else
						state <= steady_one;
						pressed <= '1';
					end if;				
			end case;
		end if;
	end process;

end behavioral;
