library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;

entity rsa_wrapper is
	generic (
		BITSIZE : integer := 64
	);
	port (
		clk                   : in  std_logic                     := '0';             --     clock.clk
		reset                 : in  std_logic                     := '0';             --     reset.reset
		mm_params_address     : in  std_logic_vector(1 downto 0)  := (others => '0'); -- mm_params.address
		mm_params_read        : in  std_logic                     := '0';             --          .read
		mm_params_readdata    : out std_logic_vector(31 downto 0);                     --          .readdata
		mm_params_write       : in  std_logic                     := '0';             --          .write
		mm_params_writedata   : in  std_logic_vector(31 downto 0)  := (others => '0'); --          .writedata
		mm_params_waitrequest : out std_logic;                                        --          .waitrequest
		st_in_data            : in  std_logic_vector(31 downto 0)  := (others => '0'); --  st_input.data
		st_in_ready           : out std_logic;                                        --          .ready
		st_in_valid           : in  std_logic                     := '0';             --          .valid
		st_in_sop             : in  std_logic                     := '0';             --          .startofpacket
		st_in_eop             : in  std_logic                     := '0';             --          .endofpacket
		st_out_data           : out std_logic_vector(31 downto 0);                    --    st_out.data
		st_out_ready          : in  std_logic                     := '0';             --          .ready
		st_out_valid          : out std_logic;                                  	  --          .valid
		st_out_sop            : out std_logic;                                        --          .startofpacket
		st_out_eop            : out std_logic;                                         --          .endofpacket
		st_out_empty          : out std_logic_vector(1 downto 0)               			--              .empty
	);
end entity rsa_wrapper;

architecture rtl of rsa_wrapper is
	component RSACypher is
		Generic (KEYSIZE: integer := 32);
		Port (indata: in std_logic_vector(KEYSIZE-1 downto 0);
				 inExp: in std_logic_vector(KEYSIZE-1 downto 0);
				 inMod: in std_logic_vector(KEYSIZE-1 downto 0);
				 cypher: out std_logic_vector(KEYSIZE-1 downto 0);
				 clk: in std_logic;
				 ds: in std_logic;
				 reset: in std_logic;
				 ready: out std_logic
				 );
	end component;
	
	-- Registar eksponenta i modula
	signal exp_reg : std_logic_vector(BITSIZE-1 downto 0);
	signal mod_reg : std_logic_vector(BITSIZE-1 downto 0);
	
	constant EXP_ADDR : std_logic_vector(1 downto 0) := "00";
	constant MOD_ADDR : std_logic_vector(1 downto 0) := "01";
	
	signal exp_strobe : std_logic;
	signal mod_strobe : std_logic;
	
	signal read_out_mux : std_logic_vector(31 downto 0);
	
		-- Broji gde se upisuju 32 bita sa magistrale podataka
	signal exp_cnt : integer range 0 to BITSIZE;
	signal mod_cnt : integer range 0 to BITSIZE;
	signal in_cnt : integer range 0 to BITSIZE;
	signal out_cnt : integer range 0 to BITSIZE;
	
	-- Masina stanja
	type state is (input_wait, input_read, rsa_start, rsa_process, output_wait, output_write);	
	signal current_state, next_state : state;
	
	signal input_read_done : std_logic;
	signal output_write_done : std_logic;
	signal output_cnt_inc : std_logic;
	
	-- Ulazni i izlazni podaci
	signal input_sample : std_logic_vector(BITSIZE-1 downto 0);
	signal output_sample : std_logic_vector(BITSIZE-1 downto 0);
	
	-- Kontrola RSA
	signal rsa_select : std_logic;
	signal rsa_ready : std_logic;
begin
	-- RSA blok
	rsa_core: RSACypher
	Generic Map(KEYSIZE => BITSIZE)
	Port Map(indata => input_sample,
				inExp => exp_reg,
				inMod => mod_reg,
				cypher => output_sample,
				clk => clk,
				ds => rsa_select,
				reset => reset,
				ready => rsa_ready);
	
	exp_strobe <= '1' when (mm_params_write = '1') and (mm_params_address = EXP_ADDR) else '0';
	mod_strobe <= '1' when (mm_params_write = '1') and (mm_params_address = MOD_ADDR) else '0';
	
	read_out_mux <= exp_reg(31 downto 0) when (mm_params_address = EXP_ADDR) else
						 mod_reg(31 downto 0) when (mm_params_address = MOD_ADDR) else
						 (others => '0');
	
	write_exp_reg: process(clk, reset)
	begin
		if (reset = '1') then
			exp_cnt <= 0;
			exp_reg <= (others => '0');
		elsif (rising_edge(clk)) then
			if (exp_strobe = '1') then
				exp_reg(exp_cnt+31 downto exp_cnt) <= mm_params_writedata;
				if(exp_cnt + 32 = BITSIZE) then
					exp_cnt <= 0;
				else
					exp_cnt <= exp_cnt + 32;
				end if;
			end if;
		end if;
	end process;
	
	write_mod_reg: process(clk, reset)
	begin
		if (reset = '1') then
			mod_cnt <= 0;
			mod_reg <= (others => '0');
		elsif (rising_edge(clk)) then
			if (mod_strobe = '1') then
				mod_reg(mod_cnt+31 downto mod_cnt) <= mm_params_writedata;
				if(mod_cnt + 32 = BITSIZE) then
					mod_cnt <= 0;
				else
					mod_cnt <= mod_cnt + 32;
				end if;
			end if;
		end if;
	end process;
	
	read_regs: process(clk, reset)
	begin
		if (reset = '1') then
			mm_params_readdata <= (others => '0');
		elsif (rising_edge(clk)) then
			mm_params_readdata <= read_out_mux;
		end if;
	end process;
	
	-- Ucitavanje podataka sa ulaza
	read_inputs: process(clk, reset)
	begin
		if (reset = '1') then
			in_cnt <= 0;
			input_read_done <= '0';
		elsif (rising_edge(clk)) then
			if (next_state = input_read) then
				input_sample(in_cnt+31 downto in_cnt) <= st_in_data;
				-- uvecavanje brojaca
				if(in_cnt + 32 = BITSIZE) then
					in_cnt <= 0;
					input_read_done <= '1';
				else
					in_cnt <= in_cnt + 32;
				end if;
			end if;		
			
			if(next_state = rsa_start) then
				input_read_done <= '0';
			end if;
		end if;
	end process;
	
	-- Ispis podataka na izlaz
	write_outputs: process(clk, reset)
	begin
		if (reset = '1') then
			out_cnt <= 0;
			output_write_done <= '0';
			output_cnt_inc <= '0';
		elsif (rising_edge(clk)) then
			if(next_state = output_write) then	
				output_cnt_inc <= '1';			
				if(out_cnt + 32 = BITSIZE) then
					out_cnt <= 0;
					output_write_done <= '1';
				else
					out_cnt <= out_cnt + 32;
				end if;
			end if;
			
			if(next_state /= output_write)then
				output_cnt_inc <= '0';
			end if;
			
			if(next_state = input_wait) then
				output_write_done <= '0';
			end if;
		end if;
	end process;
	
	-- Prelazak u sledece stanje
	control_fsm: process(clk, reset)
	begin
		if (reset = '1') then
			current_state <= input_wait;
		elsif (rising_edge(clk)) then		
			current_state <= next_state;
		end if;
	end process;
	
	-- Odredjivanje sledeceg stanja
	streaming_protocol: process(current_state, st_in_valid, st_out_ready, st_in_data, rsa_ready, input_read_done, output_write_done, output_cnt_inc)
	begin
		case current_state is
			when input_wait =>
				st_out_valid <= '0';
				st_in_ready <= '1';
				rsa_select <= '0';
				
				if (st_in_valid = '1') then
					next_state <= input_read;
				else
					next_state <= input_wait;
				end if;
			
			when input_read => 			
				st_out_valid <= '0';
				st_in_ready <= '0';
				rsa_select <= '0';
				
				if(input_read_done = '1') then
					next_state <= rsa_start;
				else
					next_state <= input_wait;
				end if;
				
			when rsa_start =>
				st_out_valid <= '0';
				st_in_ready <= '0';
				rsa_select <= '1';
				
				if (rsa_ready = '1') then
					next_state <= rsa_process;
				else
					next_state <= rsa_start;
				end if;
			
			when rsa_process =>
				st_out_valid <= '0';
				st_in_ready <= '0';
				rsa_select <= '0';
			
				if (rsa_ready = '1') then
					next_state <= output_wait;
				else
					next_state <= rsa_process;
				end if;
				
			when output_wait =>
				st_out_valid <= '1';
				st_in_ready <= '0';
				rsa_select <= '0';
				
				st_out_data <= output_sample(out_cnt+31 downto out_cnt);
				
				if (st_out_ready = '1') then
					next_state <= output_write; 
				elsif(output_write_done = '1') then
					next_state <= input_wait;
				else
					next_state <= output_wait;
				end if;
			
			when output_write => 
				st_out_valid <= '0';
				st_in_ready <= '0';
				rsa_select <= '0';
				
				if(output_write_done = '1') then
					next_state <= input_wait;
				elsif(output_cnt_inc = '1') then
					next_state <= output_wait;
				else
					next_state <= output_write;
				end if;

		end case;
				
	
	end process;

	mm_params_waitrequest <= '0';

	st_out_eop <= '0';

	st_out_sop <= '0';

	st_out_empty <= "00";

end architecture rtl; -- of linear_function