-- linear_function.vhd

-- This file was auto-generated as a prototype implementation of a module
-- created in component editor.  It ties off all outputs to ground and
-- ignores all inputs.  It needs to be edited to make it do something
-- useful.
-- 
-- This file will not be automatically regenerated.  You should check it in
-- to your version control system if you want to keep it.

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;

entity linear_function is
	port (
		clk                   : in  std_logic                     := '0';             --     clock.clk
		reset                 : in  std_logic                     := '0';             --     reset.reset
		mm_params_address     : in  std_logic_vector(1 downto 0)  := (others => '0'); -- mm_params.address
		mm_params_read        : in  std_logic                     := '0';             --          .read
		mm_params_readdata    : out std_logic_vector(7 downto 0);                     --          .readdata
		mm_params_write       : in  std_logic                     := '0';             --          .write
		mm_params_writedata   : in  std_logic_vector(7 downto 0)  := (others => '0'); --          .writedata
		mm_params_waitrequest : out std_logic;                                        --          .waitrequest
		st_in_data            : in  std_logic_vector(15 downto 0)  := (others => '0'); --  st_input.data
		st_in_ready           : out std_logic;                                        --          .ready
		st_in_valid           : in  std_logic                     := '0';             --          .valid
		st_in_sop             : in  std_logic                     := '0';             --          .startofpacket
		st_in_eop             : in  std_logic                     := '0';             --          .endofpacket
		st_out_data           : out std_logic_vector(15 downto 0);                    --    st_out.data
		st_out_ready          : in  std_logic                     := '0';             --          .ready
		st_out_valid          : out std_logic;                                  	  --          .valid
		st_out_sop            : out std_logic;                                        --          .startofpacket
		st_out_eop            : out std_logic;                                         --          .endofpacket
		st_out_empty          : out std_logic                                         --              .empty
	);
end entity linear_function;

architecture rtl of linear_function is
	signal a_reg : std_logic_vector(7 downto 0);
	signal b_reg : std_logic_vector(7 downto 0);
	
	constant A_ADDR : std_logic_vector(1 downto 0) := "00";
	constant B_ADDR : std_logic_vector(1 downto 0) := "01";
	
	signal a_strobe : std_logic;
	signal b_strobe : std_logic;
	
	signal read_out_mux : std_logic_vector(7 downto 0);
	
	type state is (read_sample, process_sample, write_sample);
	
	signal current_state, next_state : state;
	
	signal input_sample : signed(15 downto 0);
	signal output_sample : signed(23 downto 0);
	
begin
	
	a_strobe <= '1' when (mm_params_write = '1') and (mm_params_address = A_ADDR) else '0';
	b_strobe <= '1' when (mm_params_write = '1') and (mm_params_address = B_ADDR) else '0';
	
	read_out_mux <= a_reg when (mm_params_address = A_ADDR) else
						 b_reg when (mm_params_address = B_ADDR) else
						 "00000000";
	
	write_reg_a: process(clk, reset)
	begin
		if (reset = '1') then
			a_reg <= "00000010";
		elsif (rising_edge(clk)) then
			if (a_strobe = '1') then
				a_reg <= mm_params_writedata;
			end if;
		end if;
	end process;

	write_reg_b: process(clk, reset)
	begin
		if (reset = '1') then
			b_reg <= "00000011";
		elsif (rising_edge(clk)) then
			if (b_strobe = '1') then
				b_reg <= mm_params_writedata;
			end if;
		end if;
	end process;

	read_regs: process(clk, reset)
	begin
		if (reset = '1') then
			mm_params_readdata <= "00000000";
		elsif (rising_edge(clk)) then
			mm_params_readdata <= read_out_mux;
		end if;
	end process;
	
	control_fsm: process(clk, reset)
	begin
		if (reset = '1') then
			current_state <= read_sample;
		elsif (rising_edge(clk)) then
			if (next_state = process_sample) then
				input_sample <= signed(st_in_data);
			end if;
			current_state <= next_state;
		end if;
	end process;
	
	streaming_protocol: process(current_state, st_in_valid, st_out_ready, st_in_data)
	begin
		case current_state is
			when read_sample =>
				st_out_valid <= '0';
				st_in_ready <= '1';
				
				if (st_in_valid = '1') then
					next_state <= process_sample;
				else
					next_state <= read_sample;
				end if;
				
			when process_sample =>
				st_out_valid <= '0';
				st_in_ready <= '0';
				
				output_sample <= signed(a_reg)*input_sample + signed(b_reg);
				
				next_state <= write_sample;
				
			when write_sample =>
				st_out_valid <= '1';
				st_in_ready <= '0';
				
				st_out_data <= std_logic_vector(output_sample(15 downto 0));
				
				if (st_out_ready = '1') then
					next_state <= read_sample;
				else
					next_state <= write_sample;
				end if;

		end case;
				
	
	end process;

	mm_params_waitrequest <= '0';

	st_out_eop <= '0';

	st_out_sop <= '0';

	st_out_empty <= '0';

end architecture rtl; -- of linear_function
