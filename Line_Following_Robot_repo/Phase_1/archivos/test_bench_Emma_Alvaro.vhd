library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity test_Bench is
end test_Bench;

architecture tb of test_Bench is

  -- Test bench signals
	-- inputs
  signal Clk_tb   : std_logic := '0';
  signal Reset_tb : std_logic := '0';
  signal per_tb   : std_logic_vector(1 downto 0) := (others => '0');
	-- outputs
  signal led_tb : signed(7 downto 0);
  signal dac_tb : unsigned(7 downto 0);

  constant CLK_PERIOD : time := 10 ns;  -- 100 MHz => 10 ns period

begin

  -- Lab Instantiation
  Lab : entity work.GenSen
    port map(
      Clk   => Clk_tb,
      Reset => Reset_tb,
      per   => per_tb,
      led   => led_tb,
      dac   => dac_tb
    );

  -- Clock generation
  -- we create a clock with period 10ns, ie, it toggles every 5 ns
  Clk_process : process
  begin 
    Clk_tb <= '0';
    wait for CLK_PERIOD/2;
    Clk_tb <= '1';
    wait for CLK_PERIOD/2;
  end process;

  -- Main simulation process
  Stim_proc : process
  begin
    -- Reset generation, to make sure that every time we start a simulation, everything is reset
    Reset_tb <= '1';
    wait for 10*CLK_PERIOD;
    Reset_tb <= '0';

    -- 1) per = "00" 
    per_tb <= "00";
    wait for 4 ms; 

    -- 2) per = "01" 
    per_tb <= "01";
    wait for 2 ms;

    -- 3) per = "10"
    per_tb <= "10";
    wait for 909 us;

    -- 4) per = "11"
    per_tb <= "11";
    wait for 500 us;

    -- Stop simulation
    wait;
  end process;

end tb;
