library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

entity nios_test is
	port
	(
		clk50 : in  std_logic;
		LED	: out std_logic_vector(7 downto 0)
	);
end nios_test;


architecture nios_test_arch of nios_test is

	component nios_system is
		port (
			clk_clk                            : in  std_logic                    := 'X'; -- clk
			reset_reset_n                      : in  std_logic                    := 'X'; -- reset_n
			pio_led_external_connection_export : out std_logic_vector(7 downto 0)         -- export
		);
	end component nios_system;

begin

	cpu : nios_system port map (clk50, '1', LED);
	
end nios_test_arch;
